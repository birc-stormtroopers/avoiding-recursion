
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

// Direct recursion.
// The <T: Copy> is needed because we copy values into
// the result vector. For other usages we might not need it
fn inorder<T: Copy>(t: &Tree<T>, res: &mut Vec<T>) {
    // If t is not empty it is Some(node)
    if let Some(Node(left, val, right)) = t.as_deref() {
        inorder(left, res);
        res.push(*val);
        inorder(right, res);
    }
}

// Ideally, we wouldn't need to pass the vector along with the closures
// but Rust won't let us hold on to a mutable reference in a closure if
// another closure also has a mutable reference, so we pass the vector
// from continuation to continuation instead.
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

fn main() {
    let tree = Node::node(
        Node::node(
            Node::node(None, 'D', Node::leaf('H')),
            'B',
            Node::leaf('E')
        ),
        'A',
        Node::node(
            Node::leaf('F'),
            'C',
            Node::node(Node::leaf('I'), 'G', Node::leaf('J'))
        )
    );

    let mut nodes = vec![];
    inorder(&tree, &mut nodes);
    println!("{:?}", nodes);
    println!("{:?}", cps(&tree));
}
