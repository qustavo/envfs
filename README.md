envfs
=====

Want an easyway to display your environment variables? Try envfs!

Example
=====

```bash
# Build
$ make

# Run
$ mkdir env
$ ./envfs env

# Play
$ ls ./env
COLORTERM                  rvm_nightly_flag
DBUS_SESSION_BUS_ADDRESS   rvm_path
DISPLAY                    rvm_prefix
...
rvm_gemstone_package_file  XDG_SESSION_ID
rvm_gemstone_url           XDG_VTNR

$ cat env/GOPATH
/home/gchain/.go

# Cleanup
$ fusermount -u env
```
