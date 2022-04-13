# Rust implementations

Rust supports many of the things we need for continuation passing style, but then, not *quite*... Rust has closures, but you cannot define recursive closures (although you can hack it in various ugly ways), and Rust's lifetime and ownership rules can make it hard to directly translate a CPS solution into code. Very hard. When it comes to lifetime and type checking, it can be a hard language to love sometimes...

Still, with hacks, or with using functions instead of closures, we can solve the first problem, and the second can usually be solved by passing references along with functions rather than relying on values bound in closures.

I won't go into the tricks you need to get around these issues, as the C implementation shows you how to implement what you need when you have *no* language support, and it is something like that you need to default to. Anyway, in the `cps/` directory you can see how we can traverse a binary tree CPS style in Rust.

```rust
fn cps_rec<T: Copy>(t: &Tree<T>, res: &mut Vec<T>, 
                    k: Box<dyn FnOnce(&mut Vec<T>) + '_>)
{
    match t.as_deref() {
        None => { k(res) },
        Some(Node(left, val, right)) => {
            cps_rec(left, res,
                Box::new(|v| { 
                    // After we have gone left, add v and go right, 
                    // calling k when we are done
                    v.push(*val);
                    cps_rec(right, v, k);
            }));
        }
    }
}

fn cps<T: Copy>(t: &Tree<T>) -> Vec<T> {
    let mut res = vec![];
    cps_rec(t, &mut res, Box::new(|_| ()));
    res
}
```

As far as I can tell, Rust is good at doing tail-call optimisation, so if you manage to get a CPS solution to a recursion, you should be good to go (but you should probably always check, just to make sure). Otherwise, the trampoline trick works here as well.

```rust
enum Res<'a, T> {
    Done,
    More(Thunk<'a, T>),
}
use Res::*;

type Thunk<'a, T> = Box<dyn (FnOnce() -> Res<'a, T>) + 'a>;
type K<'a, T> = Box<dyn (FnOnce(&'a mut Vec<T>) -> Res<'a, T>) + 'a>;

fn cont<'a, T, F>(k: F) -> K<'a, T>
where
    F: (FnOnce(&'a mut Vec<T>) -> Res<'a, T>) + 'a,
{
    Box::new(k)
}
fn thunk<'a, T, F>(f: F) -> Thunk<'a, T>
where
    F: (FnOnce() -> Res<'a, T>) + 'a,
{
    Box::new(f)
}

fn cps<'a, T: Copy>(t: &'a Tree<T>, res: &'a mut Vec<T>, k: K<'a, T>) -> Res<'a, T> {
    match t.as_deref() {
        None => k(res),
        Some(Node(left, val, right)) => {
            let after_left = cont(move |v| {
                v.push(*val);
                cps(right, v, k)
            });
            More(thunk(move || cps(left, res, after_left)))
        }
    }
}

fn inorder<T: Copy>(t: &Tree<T>, res: &mut Vec<T>) {
    let mut res = cps(t, res, Box::new(|_| Done));
    while let More(thunk) = res {
        res = thunk()
    }
}
```

Again, the main problem with trying to implement this in Rust is the lifetime (type) annotations. Otherwise, it looks pretty much like other languages.


I am not going to implement traversals that work by moving along pointers. Pointers are tricky in Rust, as soon as they are non-trivial, and the noise would deafen the point of an example.
