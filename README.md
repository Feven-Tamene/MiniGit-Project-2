# MiniGit Diff Viewer – By Eden Yirefu

## My Role in the Project
I worked on the optional `diff <commit1> <commit2>` feature, testing, documentation, and demo presentation.

## What I Implemented
- A simple diff function that compares two commits and shows which lines are added or removed.
- It reads the stored commit files from `.minigit/objects/` and prints the line-by-line difference.

## Technologies Used
- C++
- File I/O
- Vectors and strings
- Basic hashing

## Challenges I Faced
- Understanding how to access commits from the `.minigit` system.
- Making the diff easy to read.
- Testing real examples with added, removed, or modified lines.

## Future Improvements
- Highlight word-level changes.
- Add color to make differences clearer.
- Improve formatting of diff output.

## How to Use My Feature
```
./minigit diff <commitHash1> <commitHash2>
```
Example:
```
./minigit diff 24af3d 7bc4e1
```
```
- Old line from commit1
+ New line from commit2
```
