/*
 * Configuration Stackable Filesystem
 */

#define FUSE_USE_VERSION 30

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/time.h>

#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

#include <fuse.h>

static int cfs_getattr(const char *path, struct stat *stbuf)
{
	if (lstat(path, stbuf) == -1)
		return -errno;

	return 0;
}

static int cfs_access(const char *path, int mask)
{
	if (access(path, mask) == -1)
		return -errno;

	return 0;
}

static int cfs_readlink(const char *path, char *buf, size_t size)
{
	ssize_t count;

	if ((count = readlink(path, buf, size - 1)) == -1)
		return -errno;

	buf[count] = '\0';
	return 0;
}

static int cfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
	DIR *dp;
	struct dirent *de;
	struct stat st;

	if ((dp = opendir(path)) == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		memset(&st, 0, sizeof(st));

		st.st_ino  = de->d_ino;
		st.st_mode = de->d_type << 12;

		if (filler(buf, de->d_name, &st, 0))
			break;
	}

	closedir(dp);
	return 0;
}

static int cfs_mknod(const char *path, mode_t mode, dev_t rdev)
{
	if (mknod(path, mode, rdev) == -1)
		return -errno;

	return 0;
}

static int cfs_mkdir(const char *path, mode_t mode)
{
	if (mkdir(path, mode) == -1)
		return -errno;

	return 0;
}

static int cfs_unlink(const char *path)
{
	if (unlink(path) == -1)
		return -errno;

	return 0;
}

static int cfs_rmdir(const char *path)
{
	if (rmdir(path) == -1)
		return -errno;

	return 0;
}

static int cfs_symlink(const char *from, const char *to)
{
	if (symlink(from, to) == -1)
		return -errno;

	return 0;
}

static int cfs_rename(const char *from, const char *to)
{
	if (rename(from, to) == -1)
		return -errno;

	return 0;
}

static int cfs_link(const char *from, const char *to)
{
	if (link(from, to) == -1)
		return -errno;

	return 0;
}

static int cfs_chmod(const char *path, mode_t mode)
{
	if (chmod(path, mode) == -1)
		return -errno;

	return 0;
}

static int cfs_chown(const char *path, uid_t uid, gid_t gid)
{
	if (lchown(path, uid, gid) == -1)
		return -errno;

	return 0;
}

static int cfs_truncate(const char *path, off_t size)
{
	if (truncate(path, size) == -1)
		return -errno;

	return 0;
}

static int cfs_utimens(const char *path, const struct timespec ts[2])
{
	if (utimensat(0, path, ts, AT_SYMLINK_NOFOLLOW) == -1)
		return -errno;

	return 0;
}

static int cfs_open(const char *path, struct fuse_file_info *fi)
{
	int fd;

	if ((fd = open(path, fi->flags)) == -1)
		return -errno;

	close(fd);
	return 0;
}

static int cfs_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
	int fd;
	ssize_t count;

	if ((fd = open(path, O_RDONLY)) == -1)
		return -errno;

	if ((count = pread(fd, buf, size, offset)) == -1)
		count = -errno;

	close(fd);
	return count;
}

static int cfs_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
	int fd;
	ssize_t count;

	if ((fd = open(path, O_WRONLY)) == -1)
		return -errno;

	if ((count = pwrite(fd, buf, size, offset)) == -1)
		count = -errno;

	close(fd);
	return count;
}

static int cfs_statfs(const char *path, struct statvfs *stbuf)
{
	if (statvfs(path, stbuf) == -1)
		return -errno;

	return 0;
}

static struct fuse_operations cfs_operations = {
	.getattr	= cfs_getattr,
	.access		= cfs_access,
	.readlink	= cfs_readlink,
	.readdir	= cfs_readdir,
	.mknod		= cfs_mknod,
	.mkdir		= cfs_mkdir,
	.symlink	= cfs_symlink,
	.unlink		= cfs_unlink,
	.rmdir		= cfs_rmdir,
	.rename		= cfs_rename,
	.link		= cfs_link,
	.chmod		= cfs_chmod,
	.chown		= cfs_chown,
	.truncate	= cfs_truncate,
	.utimens	= cfs_utimens,
	.open		= cfs_open,
	.read		= cfs_read,
	.write		= cfs_write,
	.statfs		= cfs_statfs,
};

int main(int argc, char *argv[])
{
	umask(0);

	return fuse_main(argc, argv, &cfs_operations, NULL);
}
