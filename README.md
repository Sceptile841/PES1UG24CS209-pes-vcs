PES-VCS — Version Control System
Name: Owaise

SRN: PES1UG24CS209

Platform: Ubuntu 22.04 (Oracle VirtualBox)

Phase 5: Branching and Checkout
Q5.1: Implementation of pes checkout <branch>
To implement checkout, the .pes/HEAD file must be updated to point to the new branch (e.g., ref: refs/heads/new-branch). Then, the working directory must be cleared of all tracked files and replaced with the files listed in the tree object associated with the target branch's latest commit. This is complex because it requires recursive directory traversal and careful handling to ensure we don't delete untracked user files or directories during the process.

Q5.2: Detecting a "Dirty Working Directory"
A "dirty" state is detected by comparing the file metadata (mtime and size) in the working directory against the entries in the Index. If the working directory version differs from the Index version, the file is modified. If that modified file also differs from the version in the target branch's tree (checked via hash comparison in the Object Store), a conflict exists, and the checkout must be aborted to prevent data loss.

Q5.3: Detached HEAD State
In a "Detached HEAD" state, the .pes/HEAD file contains a raw commit hash instead of a branch reference. If you make commits in this state, they will succeed, but no branch pointer will "follow" them. To recover these commits, a user would need to look at pes log (or the reflog in real Git), find the hash of the latest commit made, and manually create a new branch pointing to that hash using pes branch <name>.

Phase 6: Garbage Collection and Space Reclamation
Q6.1: GC Algorithm
The most efficient algorithm is Mark-and-Sweep.

Mark: Start from all known "references" (files in refs/heads/ and HEAD) and recursively traverse their trees and blobs, marking every hash found as "reachable" in a Hash Set or Bloom Filter.

Sweep: Iterate through every file in .pes/objects/ and delete any file whose hash was not marked as reachable.
Estimation: For 100k commits and 50 branches, you would visit roughly 100k commit objects plus the unique tree/blob objects they reference. Because of deduplication, the number of objects visited is significantly less than the theoretical maximum.

Q6.2: GC Race Conditions
A race condition occurs if GC starts a "sweep" while a commit operation is currently writing new objects. The GC might see a new blob that isn't yet linked to a commit/branch and delete it. When the commit finally writes the tree object referencing that blob, the blob is gone, resulting in a corrupted repository. Git avoids this by only deleting "unreachable" objects that are older than a certain grace period (usually 2 weeks), ensuring that in-progress operations have time to complete.
