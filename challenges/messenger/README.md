# messenger - LACTF 2024

Writeup TBD

tl;dr

* Overlap pages by OOB write 1 byte to pipe_buffer->page
* Free a page, but still have a reference to that page in another pipe
* spray cred to populate the page with cred_jar
* Re occupy cred_jar with fork()
* Write size integers 0's to overwrite id's in the cred struct
* Call system in one of the forked process to get flag

Requirements:
```sh
sudo apt install libcap-dev
```