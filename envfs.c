#define FUSE_USE_VERSION 30

#include <fuse.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

extern char **environ;

typedef struct {
	char *key;
	char *val;
} *var_t;

struct {
	var_t vars[4096];
} env;

// Take a environment variable like: PWD=/home/gchain
// split and copy into key and val
static void parseVar(const char *s, char **key, char **val)
{
	int i;
	int size = strlen(s);
	for(i = 0; i < size; i++) {
		if (s[i] == '=') break;
	}

	*key = malloc(i);
	*val = malloc(size - i);

	strncpy(*key, s, i);
	strcpy(*val, s + i + 1);
}

static char *getValue(const char *key)
{
	int i = 0;
	var_t var;
	while( (var = env.vars[i++]) ) {
		if (!strcmp(var->key, key)) {
			return var->val;
		}
	}
	return NULL;
}

static int envfs_getattr(const char *path, struct stat *stbuf) {
	int res = 0;

	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	} else {
		char *val = getValue(path + 1);
		if (val == NULL) {
			res = -ENOENT;
		} else {
			 stbuf->st_mode = S_IFREG | 0444;
			 stbuf->st_nlink = 1;
			 stbuf->st_size = strlen(val);
		}
	}

	return res;
}

static int envfs_open(const char *path, struct fuse_file_info *fi) {
	if (!getValue(path + 1))
		return -ENOENT;
	if ((fi->flags & 3) != O_RDONLY)
		return -EACCES;
	return 0;
}

static int envfs_read(const char *path, char *buf, size_t size, off_t offset,
		struct fuse_file_info *fi) {
	size_t len;
	char *val = getValue(path + 1);
	if (!val)
		return -ENOENT;

	 len = strlen(val);
	 if (offset < len) {
	 	 if (offset + size > len)
	 	 	 size = len - offset;
	 	 memcpy(buf, val + offset, size);
	 } else {
	 	 size = 0;
	 }
	 return size;
}

static int envfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		off_t offset, struct fuse_file_info *fi) {
	if (strcmp(path, "/") != 0)
		return -ENOENT;

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);

	int i = 0;
	var_t var;
	while( (var = env.vars[i++]) ) {
		filler(buf, var->key, NULL, 0);
	}

	return 0;
}

static struct fuse_operations envfs = {
	.getattr = envfs_getattr,
	.open    = envfs_open,
	.read    = envfs_read,
	.readdir = envfs_readdir,
};

int
main(int argc, char *argv[]) {
	// fill env struct
	int i;
	char *s = *environ;
	for (; s; i++) {
		var_t var = malloc(sizeof(var_t*));
		parseVar(s, &var->key, &var->val);
		env.vars[i] = var;
		s = *(environ+i);
	}

	return fuse_main(argc, argv, &envfs, NULL);
}