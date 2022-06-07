#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <ctype.h>
#include <windows.h>
#include <sys/stat.h>

#define FILETYPE_MAX 21
#define EXTENSION_MAXNUM 16

char *media_file[] = {
	".jpg",
	".png",
	".bmp",
	".wav",
	".mp3",
	".ico",
	"0"
};

char *media_name[] = {
	"JPEG",
	"PNG",
	"BMP",
	"WAVE",
	"MP3",
	"icon",
	"0"
};

char *code_file[] = {
	".c",
	".java",
	".py",
	".cpp",
	".hsp",
	".h",
	".txt",
	".html",
	".css",
	".js",
	".xml",
	".ini",
	"0"
};

char *code_type[] = {
	"C",
	"Java",
	"Python",
	"C++",
	"HSP",
	"C/C++/C# header",
	"Text",
	"HTML",
	"CSS",
	"Java Script",
	"XML",
	"Initialize",
	"0"
};

char *binery_file[] = {
	".exe",
	".bin",
	".pdf",
	"0"
};

char *binery_name[] = {
	"Application",
	"Binery",
	"PDF",
	"0"
};

typedef struct { unsigned int w, h; } Size;

// fsrc
int FNAME_MAX = 24;
int COLUMN_SIZE = 4;
char USE_COLOR = 1;
WORD DIR_COLOR = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
WORD LINK_COLOR = FOREGROUND_BLUE | FOREGROUND_INTENSITY;

int ls_normal(char *filepath);
int ls_info(char *filepath, char info[8]);
int ls_size(char *filepath);
void set_fsrc();
void out_filetype(struct dirent *dp);
void out_picsize(struct dirent *dp, char *filepath);
void out_filesize(struct dirent *dp, char *filepath);
int ext_exist(char ext_array[][16], char *extension);
int media_file_number(char *filename);
int code_file_number(char *filename);
int binery_file_number(char *filename);
Size get_jpeg_size(const char *jpg);
Size get_png_size(const char *png);
Size get_bmp_size(const char *bmp);
unsigned get_digit(unsigned num);
char *get_extension(char *filename);

char strong_ext[EXTENSION_MAXNUM][16];
char strong = 0;
char only_ext[EXTENSION_MAXNUM][16];
char only = 0;

int main(int argc, char *argv[]){
	int opt;
	char filepath[128] = ".";
	char listtype[8] = {0};
	char info = 0;
	int strong_ext_index = 0;
	int only_ext_index = 0;
	int i;

	memset(strong_ext, 0, EXTENSION_MAXNUM * 16);
	memset(only_ext, 0, EXTENSION_MAXNUM * 16);

	set_fsrc();

	for(i = 1; i < argc; i++){
		if(argv[i][0] != '-'){
			strncpy(filepath, argv[i], 128);
		}else{
			if(argv[i][1] == 's' || argv[i][1] == 'o'){
				i++;
			}
		}
	}

	while((opt = getopt(argc, argv, "ltgLfs:o:h")) != -1){
		switch(opt){
			case 'l':
				info = 1;
				break;
			case 't':
				info = 1;
				listtype[0] = 1;
				break;
			case 'g':
				info = 1;
				listtype[1] = 1;
				break;
			case 'L':
				info = 1;
				listtype[2] = 1;
				break;
			case 'f':
				info = 1;
				listtype[3] = 1;
				break;
			case 's':
				strong = 1;
                if(strong_ext_index > EXTENSION_MAXNUM) {
                    fprintf(stderr, "can't specify more than 16 extensions.\n");
                    exit(-1);
                }
				strcpy(strong_ext[strong_ext_index], optarg);
				strong_ext_index++;
				break;
			case 'o':
				only = 1;
                if(only_ext_index > EXTENSION_MAXNUM) {
                    fprintf(stderr, "can't specify more than 16 extensions.\n");
                    exit(-1);
                }
				strcpy(only_ext[only_ext_index], optarg);
				only_ext_index++;
				break;
			case 'h':
			default:
				puts("Usage: fs [directryPath] [-option]");
				puts("Options:");
				puts("-l : View in list format");
				puts("-t : View file type.");
				puts("-g : View image file width and height.");
				puts("-L : View file or folder size.");
				puts("-f : View full path.");
				puts("-s [extension] : Highlight files with specified extensions.");
				puts("-o [extension] : Show only files with specified extensions.");
				return 0;
				break;
		}
	}
	if(info == 0) return ls_normal(filepath);
	else return ls_info(filepath, listtype);
}

int ls_normal(char *filepath){
	DIR *dir;
	struct dirent *dp;
	int i, j;
	HANDLE hc = GetStdHandle(STD_OUTPUT_HANDLE);
	char *extension;

	if((dir = opendir(filepath)) == NULL){
		if(strncmp(filepath, ".", 128) == 0)fprintf(stderr, "current directry cannot open.\n");
		else fprintf(stderr, "%s cannot open.\n", filepath);
		return -1;
	}

	for(j = 0, i = 1; j < 2; j++, seekdir(dir, 0)){
		for(dp = readdir(dir); dp != NULL; dp = readdir(dir)){
			if(strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)continue;
			extension = get_extension(dp->d_name);
			if(extension != NULL){
				if(only && ext_exist(only_ext, extension + 1) != 0)continue;
			}else{
				if(dp->d_type == DT_DIR){
					if(only && ext_exist(only_ext, "dir") != 0)continue;
				}else{
					if(only && ext_exist(only_ext, "nope") != 0)continue;
				}
			}
			if(dp->d_type == DT_DIR){
				if(j == 1)continue;
				if(USE_COLOR){
					if(ext_exist(strong_ext, "dir") == 0 && strong)SetConsoleTextAttribute(hc, BACKGROUND_GREEN | BACKGROUND_RED);
					else SetConsoleTextAttribute(hc, DIR_COLOR);
				}
				printf("-%s", dp->d_name);
			}else{
				if(j == 0)continue;
				if(extension != NULL){
					if(USE_COLOR && (strcmp(extension, ".lnk") == 0 || strcmp(extension, ".url") == 0)){
						if(ext_exist(strong_ext, extension + 1) == 0 && strong)SetConsoleTextAttribute(hc, LINK_COLOR | BACKGROUND_GREEN | BACKGROUND_RED);
						else SetConsoleTextAttribute(hc, LINK_COLOR);
						printf(" %s", dp->d_name);
					}else{
						if(ext_exist(strong_ext, extension + 1) == 0 && strong)SetConsoleTextAttribute(hc, BACKGROUND_GREEN | BACKGROUND_RED);
						printf(" %s", dp->d_name);
					}
				}else{
					if(ext_exist(strong_ext, "nope") == 0 && strong)SetConsoleTextAttribute(hc, BACKGROUND_GREEN | BACKGROUND_RED);
					printf(" %s", dp->d_name);
				}
			}

			int j, spacenum;
			for(j = 1; (spacenum = FNAME_MAX * j - ((int)strlen(dp->d_name) + 1) + (j >= 2 ? 1 : 0)) < 0;j++){
				i++;
				if(i >= COLUMN_SIZE){
					i = 0;
					break;
				}
			}
			for(j = 0; j < spacenum; j++){
				printf(" ");
			}

			if(USE_COLOR)SetConsoleTextAttribute(hc, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			if(i % COLUMN_SIZE == 0){
				i = 0;
				printf("\n");
			}else{
				printf("|");
			}

			i++;
		}
	}
	closedir(dir);
	return 0;
}

int ls_info(char *filepath, char info[8]){
	DIR *dir;
	struct dirent *dp;
	int i, j;
	int filename_length;
	char *extension;
	HANDLE hc = GetStdHandle(STD_OUTPUT_HANDLE);
	char fullpath[FILENAME_MAX];
	char tmpbuf[FILENAME_MAX];
	char filename[FILENAME_MAX];

	if((dir = opendir(filepath)) == NULL){
		if(strncmp(filepath, ".", 128) == 0)fprintf(stderr, "current directry cannot open.\n");
		else fprintf(stderr, "%s cannot open.\n", filepath);
		return -1;
	}

	filename_length = FNAME_MAX;
	for(dp = readdir(dir); dp != NULL; dp = readdir(dir)){
		if(info[3]){
            snprintf(tmpbuf, sizeof(tmpbuf), "%s\\%s", filepath, dp->d_name);
			GetFullPathName(tmpbuf, sizeof(fullpath), fullpath, NULL);
			if(filename_length < (int)strlen(fullpath))filename_length = (int)strlen(fullpath);
		}else{
			if(filename_length < (int)strlen(dp->d_name))filename_length = (int)strlen(dp->d_name);
		}
	}
	seekdir(dir, 0);

	for(i = 0; i < (filename_length - 8) / 2; i++)printf("-");
	printf("filename");
	for(i = 0; i < filename_length - (filename_length - 8) / 2 - 8; i++)printf("-");
	if(info[0]){
		printf("|");
		for(i = 0; i < (FILETYPE_MAX + 1 - 8) / 2; i++)printf("-");
		printf("filetype");
		for(i = 0; i < FILETYPE_MAX + 1 - (FILETYPE_MAX + 1 - 8) / 2 - 8; i++)printf("-");
	}
	if(info[1]){
		printf("|-width--");
		printf("|-height-");
	}
	if(info[2]){
		printf("|--Length---");
	}
	printf("|\n");

	for(j = 0; j < 2; j++, seekdir(dir, 0)){
		for(dp = readdir(dir); dp != NULL; dp = readdir(dir)){
			if(strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)continue;
			extension = get_extension(dp->d_name);
			if(extension != NULL){
				if(only && ext_exist(only_ext, extension + 1) != 0)continue;
			}else{
				if(dp->d_type == DT_DIR){
					if(only && ext_exist(only_ext, "dir") != 0)continue;
				}else{
					if(only && ext_exist(only_ext, "nope") != 0)continue;
				}
			}

			if(info[3]){
                snprintf(tmpbuf, sizeof(tmpbuf), "%s\\%s", filepath, dp->d_name);
                GetFullPathName(tmpbuf, sizeof(filename), filename, NULL);
			}else{
				strncpy(filename, dp->d_name, FILENAME_MAX);
			}

			if(dp->d_type == DT_DIR){
				if(j == 1)continue;
				if(USE_COLOR){
					if(ext_exist(strong_ext, "dir") == 0 && strong)SetConsoleTextAttribute(hc, BACKGROUND_GREEN | BACKGROUND_RED);
					else SetConsoleTextAttribute(hc, DIR_COLOR);
				}
				printf("%s", filename);
			}else{
				if(j == 0)continue;
				if(extension != NULL){
					if(USE_COLOR && (strcmp(extension, ".lnk") == 0 || strcmp(extension, ".url") == 0)){
						if(ext_exist(strong_ext, extension + 1) == 0 && strong)SetConsoleTextAttribute(hc, LINK_COLOR | BACKGROUND_GREEN | BACKGROUND_RED);
						else SetConsoleTextAttribute(hc, LINK_COLOR);
						printf("%s", filename);
					}else{
						if(ext_exist(strong_ext, extension + 1) == 0 && strong)SetConsoleTextAttribute(hc, BACKGROUND_GREEN | BACKGROUND_RED);
						printf("%s", filename);
					}
				}else{
					if(ext_exist(strong_ext, "nope") == 0 && strong)SetConsoleTextAttribute(hc, BACKGROUND_GREEN | BACKGROUND_RED);
					printf("%s", filename);
				}
			}
			for(i = 0; i < filename_length - (int)strlen(filename); i++){
				printf(" ");
			}
			if(USE_COLOR)SetConsoleTextAttribute(hc, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			if(info[0])out_filetype(dp);
			if(info[1])out_picsize(dp, filepath);
			if(info[2])out_filesize(dp, filepath);
			printf("|\n");
		}
	}
	closedir(dir);
	return 0;
}

void out_filetype(struct dirent *dp){
	int i;
	char *extension;
	char fileinfo[FILENAME_MAX];

	if(dp->d_type == DT_DIR){
		snprintf(fileinfo, FILENAME_MAX, "directry");
	}else{
		if((extension = get_extension(dp->d_name)) != NULL){
			if(USE_COLOR && (strcmp(extension, ".lnk") == 0 || strcmp(extension, ".url") == 0)){
				snprintf(fileinfo, FILENAME_MAX, "shortcut");
			}else{
				if(media_file_number(dp->d_name) != -1){
					snprintf(fileinfo, FILENAME_MAX, "%s file", media_name[media_file_number(dp->d_name)]);
				}else if(code_file_number(dp->d_name) != -1){
					snprintf(fileinfo, FILENAME_MAX, "%s file", code_type[code_file_number(dp->d_name)]);
				}else if(binery_file_number(dp->d_name) != -1){
					snprintf(fileinfo, FILENAME_MAX, "%s file", binery_name[binery_file_number(dp->d_name)]);
				}else{
					snprintf(fileinfo, FILENAME_MAX, "file");
				}
			}
		}else{
			snprintf(fileinfo, FILENAME_MAX, "file");
		}
	}

	printf("| %s", fileinfo);
	for(i = 0; i < FILETYPE_MAX - (int)strlen(fileinfo); i++){
		printf(" ");
	}
}

void out_picsize(struct dirent *dp, char *filepath){
	int i;
	char filepathname[FILENAME_MAX];
	Size size;

	size.h = 0;
	size.w = 0;
	if(media_file_number(dp->d_name) == 0){ //JPEG
		snprintf(filepathname, FILENAME_MAX, "%s/%s", filepath, dp->d_name);
		size = get_jpeg_size(filepathname);
		printf("| %d", size.w);
		for(i = 0; i < 7 - (int)get_digit(size.w); i++){
			printf(" ");
		}
		printf("| %d", size.h);
		for(i = 0; i < 7 - (int)get_digit(size.h); i++){
			printf(" ");
		}
	}else if(media_file_number(dp->d_name) == 1){ //PNG
		snprintf(filepathname, FILENAME_MAX, "%s/%s", filepath, dp->d_name);
		size = get_png_size(filepathname);
		printf("| %d", size.w);
		for(i = 0; i < 7 - (int)get_digit(size.w); i++){
			printf(" ");
		}
		printf("| %d", size.h);
		for(i = 0; i < 7 - (int)get_digit(size.h); i++){
			printf(" ");
		}
	}else if(media_file_number(dp->d_name) == 2){//BMP
		snprintf(filepathname, FILENAME_MAX, "%s/%s", filepath, dp->d_name);
		size = get_bmp_size(filepathname);
		printf("| %d", size.w);
		for(i = 0; i < 7 - (int)get_digit(size.w); i++){
			printf(" ");
		}
		printf("| %d", size.h);
		for(i = 0; i < 7 - (int)get_digit(size.h); i++){
			printf(" ");
		}
	}else{
		printf("|");
		for(i = 0; i < 8; i++){
			printf(" ");
		}
		printf("|");
		for(i = 0; i < 8; i++){
			printf(" ");
		}
	}
}

unsigned long long get_file_size(char *filepath){
	struct stat sb;

	if(stat(filepath, &sb) != 0){
		fprintf(stderr, "%s cannnot open\n", filepath);
		return 0;
	}

	return sb.st_size;
}

unsigned long long get_dir_size(char *folderpath){
	struct dirent *dp;
	char pathname[FILENAME_MAX];
	DIR *dir;
	unsigned long long size = 0;
	if((dir = opendir(folderpath)) == NULL){
		fprintf(stderr, "\"%s\" cannot open\n", folderpath);
		return 0;
	}
	for(dp = readdir(dir); dp != NULL; dp = readdir(dir)){
		if(strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)continue;
		snprintf(pathname, FILENAME_MAX, "%s/%s", folderpath, dp->d_name);
		if(dp->d_type == DT_DIR){
			size += get_dir_size(pathname);
		}else{
			size += get_file_size(pathname);
		}
	}
	closedir(dir);
	return size;
}

void out_filesize(struct dirent *dp, char *filepath){
	int i;
	char filepathname[FILENAME_MAX];
	unsigned long long size;

	snprintf(filepathname, FILENAME_MAX, "%s/%s", filepath, dp->d_name);
	if(dp->d_type == DT_DIR){
		size = get_dir_size(filepathname);
	}else{
		size = get_file_size(filepathname);
	}
	printf("| %I64d", size);
	for(i = 0; i < 10 - (int)get_digit(size); i++){
		printf(" ");
	}
}

int ext_exist(char ext_array[][16], char *extension){
	int i;

	for(i = 0; i < EXTENSION_MAXNUM; i++){
		if(strcmp(ext_array[i], extension) == 0){
			return 0;
		}
	}

	return -1;
}

int media_file_number(char *filename){
	int i;
	char *extension;
	if((extension = get_extension(filename)) == NULL)return -1;
	for(i = 0; strcmp(media_file[i], "0") != 0; i++){
		if(strcmp(extension, media_file[i]) == 0)return i;
	}
	return -1;
}

int code_file_number(char *filename){
	int i;
	char *extension;
	if((extension = get_extension(filename)) == NULL)return -1;
	for(i = 0; strcmp(code_file[i], "0") != 0; i++){
		if(strcmp(extension, code_file[i]) == 0)return i;
	}
	return -1;
}

int binery_file_number(char *filename){
	int i;
	char *extension;
	if((extension = get_extension(filename)) == NULL)return -1;
	for(i = 0; strcmp(binery_file[i], "0") != 0; i++){
		if(strcmp(extension, binery_file[i]) == 0)return i;
	}
	return -1;
}

Size get_jpeg_size(const char *jpg){
	Size ret = {0, 0};
	unsigned char buf[8];
	FILE *f = fopen(jpg, "rb");
	while (f && fread(buf, 1, 2, f) == 2 && buf[0] == 0xff){
		if (buf[1] >= 0xc0 && buf[1] <= 0xcf && fread(buf, 1, 7, f) == 7){
			ret.h = buf[3] * 256 + buf[4];
			ret.w = buf[5] * 256 + buf[6];
		}else if (buf[1] == 0xd8 || (fread(buf, 1, 2, f) == 2 && !fseek(f, buf[0] * 256 + buf[1] - 2, SEEK_CUR))) continue;
		break;
	}
	if (f) fclose(f);
	return ret;
}

Size get_png_size(const char *png){
	Size ret = {0, 0};
	unsigned char buf;
	int correct = 0;
	int i;
	FILE *f = fopen(png, "rb");
	while(fread(&buf, sizeof(char), 1, f) >= 1){
		if(correct == 0 && buf == 'I')correct++;
		if(correct == 1 && buf == 'H')correct++;
		if(correct == 2 && buf == 'D')correct++;
		if(correct == 3 && buf == 'R'){
			for(i = 0; i < 4; i++){
				fread(&buf, sizeof(char), 1, f);
				ret.w = ret.w | (unsigned int)(buf << (3 - i) * 8);
			}
			for(i = 0; i < 4; i++){
				fread(&buf, sizeof(char), 1, f);
				ret.h = ret.h | (unsigned int)(buf << (3 - i) * 8);
			}
			break;
		}
		if(correct > 0 && buf != 'I' && buf != 'H' && buf != 'D' && buf != 'R')correct = 0;
	}

	if(f) fclose(f);
	return ret;
}

Size get_bmp_size(const char *bmp){
	Size ret = {0, 0};
	unsigned char buf;
	int i;
	FILE *f = fopen(bmp, "rb");
	fseek(f, 18, SEEK_SET);
	for(i = 0; i < 4; i++){
		fread(&buf, sizeof(char), 1, f);
		ret.w = ret.w | (unsigned int)(buf << i * 8);
	}
	for(i = 0; i < 4; i++){
		fread(&buf, sizeof(char), 1, f);
		ret.h = ret.h | (unsigned int)(buf << i * 8);
	}

	if(f) fclose(f);
	return ret;
}

unsigned get_digit(unsigned num){
    unsigned digit=0;
	if(num == 0)return 1;
    while(num!=0){
        num /= 10;
        digit++;
    }
    return digit;
}

char *get_extension(char *filename){
	int i;
	char *next_dot;
	for(i = 1; filename[i] != '\0'; i++){
		if(filename[i] == '.'){
			if((next_dot = get_extension(filename + i)) != NULL)return next_dot;
			else return filename + i;
		}
	}
	return NULL;
}

char *scan(FILE *fp){
	static char buf[64];
	char next;
	int type = 0;
	int i;

	memset(buf, (int)'\0', 64);

	for(i = 0; i < 63;){
		next = fgetc(fp);
		if(next == EOF && i == 0)return NULL;
		if(next == EOF) return buf;
		if(type == 0 && (next == ' ' || next == '\n')) continue;
		if(type != 0 && (next == ' ' || next == '\n')) return buf;
		if(type == 0){
			if(isalpha(next)){
				type = 1;// alphabet
			}else if(isdigit(next)){
				type = 2;// number
			}else{
				type = 3;// cymbol
			}
		}else{
			if((type == 1 && (isalpha(next) == 0 && next != '_')) || (type == 2 && isdigit(next) == 0) || (type == 3 && isalnum(next))){
				ungetc(next, fp);
				return buf;
			}
		}
		buf[i] = next;
		i++;
	}
	return buf;
}

void set_fsrc(){
	FILE *fp;
	char *token;
	char *fsrc_elem[] = {
		"FILENAME_MAX",
		"COLUMN_SIZE",
		"USE_COLOR",
		"DIR_COLOR",
		"LINK_COLOR",
		"0"
	};
	char userProfile[128];
	char path[128];
	int elm_num;

	ExpandEnvironmentStrings("%USERPROFILE%", userProfile, 128);
	snprintf(path, 128, "%s/.fsrc", userProfile);
	if((fp = fopen(path, "r")) == NULL){
		return;
	}

	while((token = scan(fp)) != NULL){
		for(elm_num = 0; strcmp(fsrc_elem[elm_num], "0") != 0; elm_num++){
			if(strcmp(token, fsrc_elem[elm_num]) == 0) break;
		}
		if(elm_num == 5){
			puts("Illegal token has detected in .fsrc!");
			return;
		}
		token = scan(fp);
		if(token == NULL)return;
		if(strcmp(token, "=") != 0){
			puts("None '=' token after the element.");
			return;
		}
		if((token = scan(fp)) == NULL)return;
		if(elm_num == 0){
			FNAME_MAX = atoi(token);
		}else if(elm_num == 1){
			COLUMN_SIZE = atoi(token);
		}else if(elm_num == 2){
			if(strcmp(token, "TRUE") == 0)USE_COLOR = 1;
			if(strcmp(token, "FALSE") == 0)USE_COLOR = 0;
		}else if(elm_num == 3){
			if(strcmp(token, "RED") == 0)DIR_COLOR = FOREGROUND_RED | FOREGROUND_INTENSITY;
			if(strcmp(token, "BLUE") == 0)DIR_COLOR = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
			if(strcmp(token, "GREEN") == 0)DIR_COLOR = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
			if(strcmp(token, "YELLOW") == 0)DIR_COLOR = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
			if(strcmp(token, "SKYBLUE") == 0)DIR_COLOR = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
			if(strcmp(token, "PURPLE") == 0)DIR_COLOR = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
			if(strcmp(token, "WHITE") == 0)DIR_COLOR = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		}else if(elm_num == 4){
			if(strcmp(token, "RED") == 0)LINK_COLOR = FOREGROUND_RED | FOREGROUND_INTENSITY;
			if(strcmp(token, "BLUE") == 0)LINK_COLOR = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
			if(strcmp(token, "GREEN") == 0)LINK_COLOR = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
			if(strcmp(token, "YELLOW") == 0)LINK_COLOR = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
			if(strcmp(token, "SKYBLUE") == 0)LINK_COLOR = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
			if(strcmp(token, "PURPLE") == 0)LINK_COLOR = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
			if(strcmp(token, "WHITE") == 0)LINK_COLOR = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		}
	}
}