fn main() {
    let xs: Vec<String> = vec!["a".to_string(),"b".to_string()];
    // N.B. fuse() doesn't 'cap off' cycle.
    for x in xs.iter().cycle().fuse() {
        println!("{:?}", x);
    }
}
