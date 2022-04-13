// A node is a tuple of (left, value, right)
struct Node<T>(pub Tree<T>, pub T, pub Tree<T>);
// A tree is an optional pointer to a node
type Tree<T> = Option<Box<Node<T>>>;

impl<T> Node<T> {
    fn node(left: Tree<T>, val: T, right: Tree<T>) -> Tree<T> {
        Some(Box::new(Node(left, val, right)))
    }
    fn leaf(val: T) -> Tree<T> {
        Node::node(None, val, None)
    }
}

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

fn main() {
    let tree = Node::node(
        Node::node(Node::node(None, 'D', Node::leaf('H')), 'B', Node::leaf('E')),
        'A',
        Node::node(
            Node::leaf('F'),
            'C',
            Node::node(Node::leaf('I'), 'G', Node::leaf('J')),
        ),
    );

    let mut nodes = vec![];
    inorder(&tree, &mut nodes);
    println!("{:?}", nodes);
}
