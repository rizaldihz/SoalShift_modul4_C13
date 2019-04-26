#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <time.h>
#include <grp.h>
#include <pwd.h>

static const char *dirpath = "/home/duhbuntu/shift4";
char key[97] = "qE1~ YMUR2\"`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|\\8s;g<{3.u*W-0";

void enc(char* yangdienc)
{
	if(!strcmp(yangdienc,".") || !strcmp(yangdienc,"..")) return;
	for(int i=0;i<strlen(yangdienc);i++)
	{
		for(int j=0;j<94;j++){
			// printf("%c",key[j]);
			if(yangdienc[i]==key[j]){
				yangdienc[i] = key[(j+17)%94];
				break;
			}
		}
	}
}

void dec(char* yangdidec)
{
	if(!strcmp(yangdidec,".") || !strcmp(yangdidec,"..")) return;
	for(int i=0;i<strlen(yangdidec);i++)
	{
		for(int j=0;j<94;j++){
			// printf("%c",key[j]);
			if(yangdidec[i]==key[j]){
				yangdidec[i] = key[(j+77)%94];
				break;
			}
		}
	}
}

static void* pre_init(struct fuse_conn_info *conn){
        char folder[100000] = "/Videos";
				enc(folder);
				char fpath[1000];
    	 	sprintf(fpath,"%s%s", dirpath, folder);
				mkdir(fpath,0755);
        (void) conn;
        return NULL;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
       int res;
			 enc(path);
			 char fpath[1000];

    	 sprintf(fpath,"%s%s", dirpath, path);
       res = mkdir(fpath, mode);
			 printf("%s",fpath);
       if (res == -1)
              return -errno;
      return 0;
}

// static void* destroy(){

// }

static int xmp_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
	int fd;
	int res;
	char fpath[1000];
	enc(path);
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);

	(void) fi;
	
	fd = open(fpath, O_WRONLY);
	if (fd == -1)
		return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}

static int xmp_chmod(const char *path, mode_t mode)
{
	int res;
	char fpath[1000];
	enc(path);
	sprintf(fpath,"%s%s",dirpath,path);
    res = chmod(fpath, mode);
    if (res == -1)
        return -errno;
    return 0;
}

static int xmp_getattr(const char *path, struct stat *stbuf)
{
  int res;
	char fpath[1000];
	char old[1000];
	strcpy(old,path);
	enc(path);	// enc(fpath);
	sprintf(fpath,"%s%s",dirpath,path);
	res = lstat(fpath, stbuf);
	 struct passwd *name = getpwuid(stbuf->st_uid);
    struct group *grup = getgrgid(stbuf->st_gid);
		if((!strcmp(name->pw_name,"chipset") || !strcmp(name->pw_name,"ic_controller"))&& !strcmp(grup->gr_name,"rusak") 
				&& (stbuf->st_mode & S_IRUSR == 0))
				{
					char root[1000];
					strcpy(root,dirpath);
					char note[10000] = "/filemiris.txt";
					enc(note);
					strcat(root,note);
					FILE * fp;
   				fp = fopen (root, "a+");
					char t[20];
					time_t now = time(NULL);
					strftime(t, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));
					char buffer[1000];
					sprintf(buffer,"%s-%s-%s-%s",old,name->pw_name,grup->gr_name,t);
					fprintf(fp,"%s\n",buffer);
					remove(fpath);
					fclose(fp);
				}
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
  char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else {
		enc(path);
		sprintf(fpath, "%s%s",dirpath,path);
	}
	int res = 0;

	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

    char katak[100000];
		while ((de = readdir(dp)) != NULL) {
		struct stat st;
    memset(katak,0,100000);
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
    strcpy(katak,de->d_name);
		dec(katak);
		res = (filler(buf, katak, &st, 0));
			if(res!=0) break;
	}

	closedir(dp);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
  char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		enc(path);
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else{
		enc(path);
		sprintf(fpath, "%s%s",dirpath,path);
	}
	int res = 0;
    int fd = 0 ;
   
    
	    (void) fi;
	    fd = open(fpath, O_RDONLY);
	    if (fd == -1)
	    	return -errno;

	    res = pread(fd, buf, size, offset);
	    if (res == -1)
	    	res = -errno;

        // printf("%s\n",buf);
	    close(fd);
	    return res;
    
}

static int xmp_open(const char *path, struct fuse_file_info *fi)
{
	int res;
	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		enc(path);
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else{
		enc(path);
		sprintf(fpath, "%s%s",dirpath,path);
	}
	res = open(fpath, fi->flags);
	if (res == -1)
		return -errno;

	close(res);
	return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
	int res;
	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		enc(path);
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else{
		enc(path);
		sprintf(fpath, "%s%s",dirpath,path);
	}
	
	res = mknod(fpath, mode, rdev);
    if(res == -1)
        return -errno;

    return 0;
	
}

static int xmp_truncate(const char *path, off_t size)
{
	int res;
	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		enc(path);
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else{
		enc(path);
		sprintf(fpath, "%s%s",dirpath,path);
	}
	res = truncate(fpath, size);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_utimens(const char *path, const struct timespec ts[2])
{
	int res;
	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		enc(path);
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else{
		enc(path);
		sprintf(fpath, "%s%s",dirpath,path);
	}
	/* don't use utime/utimes since they follow symlinks */
	res = utimensat(0, fpath, ts, AT_SYMLINK_NOFOLLOW);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rename(const char *from, const char *to)
{
	int res;
	char fpathfrom[1000];
	char fpathto[1000];
	enc(from);
	enc(to);
	sprintf(fpathfrom,"%s%s",dirpath,from);
	sprintf(fpathto,"%s%s",dirpath,to);
	res = rename(fpathfrom, fpathto);
	if (res == -1)
		return -errno;

	return 0;
}


static int xmp_unlink(const char *path)
{
	int res;
    char fpath[1000];
		enc(path);
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	res = unlink(fpath);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rmdir(const char *path)
{
	int res;
    char fpath[1000];
		enc(path);
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	res = rmdir(fpath);
	if (res == -1)
		return -errno;

	return 0;
}

static struct fuse_operations xmp_oper = {
	.init 		= pre_init,
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.read		= xmp_read,
	.mkdir = xmp_mkdir,
	.write = xmp_write,
	.open = xmp_open,
	// .create = xmp_create,
	// .release = xmp_release,
	.chmod = xmp_chmod,
	.truncate = xmp_truncate,
	.mknod = xmp_mknod,
	.utimens = xmp_utimens,
	.rename = xmp_rename,
};

int main(int argc, char *argv[])
{
	// char namafile[]="coba.txt";
	// enc(namafile);
	// printf("%s\n",namafile);
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}