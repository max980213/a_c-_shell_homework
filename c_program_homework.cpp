#include <fstream>
#include <string>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <pwd.h>
#include <memory.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
//#include <direct.h>
//Header files might be used

using namespace std;

#define MAX_LINE 80//longest cmd
#define MAX_NAME_LEN 100//longest username
#define MAX_PATH_LEN 1000//longest path

//declare global variables
extern char **environ;//environment variables
char *cmd_array[MAX_LINE/2+1];//inputing cmd
int pipe_fd[2];//something about pipe,dont want to use it
int cmd_cnt;//the number of strings in each cmd

//declare functions
//basic functions
void readcommand();//read cmd
int is_internal_cmd();//internal cmd
int is_pipe();//analyse pipe
void do_redirection();//sth about redirection

//self defined functions
void weicome();//welcome screen
void printprompt();//print cmd with path
int getcommandlen();//calculate the length of each cmd
void do_pipe(int pos);//do pipe cmd
void run_external_cmd(int pos);//do external cmd
int is_bg_cmd();//check background cmd
void myquit();//just quit this shell
void myexit();//exit directly
void myclear();//like clear in bash
void print_continue_info();//print information about continue(dont know what that means)
void mypwd();//like pwd in bash
void myecho();//like echo in bash
void echo_redirect();//echo with rediction
void mytime();//like time in bash
void mycd();//like cd in bash
void myhelp();//iike..help
void help_redirect();//help with redirection
void print_manual();//print user nanual,the son of help()
void mybatch();//sth like bat
void mydir();//like dir or ls in bash
void mydir_redirect();//with redirection
void myls();//like ls in bash
void mycat();
void mymkdir();
void mywordcount();//print the number of charactors of files

//functions realised

void readcommand()//to read input cmd
{
    int cnt = 0;//record the number of strings in cmd_array[]
    char str[MAX_LINE];
	char *helper;
	memset(cmd_array,0,MAX_LINE/2+1);//clear it each times
	fgets(str,MAX_LINE,stdin);
//fgets is safer cuz it checks for overflow
	if(str[strlen(str)-1]=='\n')
	{
		str[strlen(str)-1]='\0';//replace'\n'with '\0'
	}
	helper=strtok(str," ");//use space to takeapart cmd
	//function strtok is used to devide str into pieces with " "
	//returns a pointer towards the first str
	//returns null at the last one
	while(helper!=NULL)//write into cmd_array
	{
		cmd_array[cnt]=(char*)malloc(sizeof(*helper));//havent initialize it so malloc it
		strcpy(cmd_array[cnt++],helper);
		helper=strtok(NULL," ");
		//there are two pointers in this function
		//the first one pointera points to the first str
		//the second one points two the place where " " exists
//if the first parameter is NULL,begin checking from pointerb,not the begining
//so,the first time use it is to get the pointer poingting at the first str
//and then use it in the loops to get the latter strs
	}
	cmd_cnt=cnt;
}

int is_internal_cmd()//to check if the cmd belongs to this shell
                     //and use different function
{
	if(cmd_array[0]==NULL)
	{
		return 0;//if no cmd inputs,do nothing
	}
	else if(strcmp(cmd_array[0],"bye")==0)
	{
		myquit();
	}
	else if(strcmp(cmd_array[0],"exit")==0)
	{
		myexit();
	}//quit and exit
	else if(strcmp(cmd_array[0],"cat")==0)
	{
		mycat();
		return 1;
	}
	else if(strcmp(cmd_array[0],"clear")==0)
	{
		myclear();
		return 1;
	}
	//else if(strcmp(cmd_array[0],"continue")==0)
	//{
		//print_continue_info();
		//return 1;
	//}//the continue cmd only works in loops
	else if(strcmp(cmd_array[0],"pwd")==0)
	{
		mypwd();
		return 1;
	}
	else if(strcmp(cmd_array[0],"echo")==0)
	{
		//check redirection option   work on it later
		myecho();
		return 1;
	}
	else if(strcmp(cmd_array[0],"time")==0)
	{
		mytime();
		return 1;
	}
	//else if(strcmp(cmd_array[0],"fenviron")==0)
	else if(strcmp(cmd_array[0],"cd")==0)
	{
		mycd();
		return 1;
	}
	//else if(strcmp(cmd_array[0],"help")==0)
	//else if(strcmp(cmd_array[0],"exec")==0)
	//else if(strcmp(cmd_array[0],"umask")==0)
	//else if(strcmp(cmd_array[0],"fg")==0)
	//else if(strcmp(cmd_array[0],"bg")==0)
	else if(strcmp(cmd_array[0],"myshell")==0)//bat(pi chu li)
	{
		if(cmd_cnt==1)//only one cmd input
		{
			printf("myshell: myshell: too few arguments\n");
			return 1;
		}
		else if (cmd_cnt==2)
		{
			mybatch();
			return 1;
		}
		else
		{
			printf("myshell: myshell: too many arguments\n");
			return 0;
		}
	}
	else if(strcmp(cmd_array[0],"dir")==0)
		//redirect option
	{
		mydir();
		return 1;
	}
	else if(strcmp(cmd_array[0],"ls")==0)
	{
		myls();
		return 1;
	}
	else if(strcmp(cmd_array[0],"mkdir")==0)
	{
		mymkdir();
		return 1;
	}
	else if(strcmp(cmd_array[0],"wc")==0)
	{
		mywordcount();
		return 1;
	}
	else
	{
		printf("myshell: command not found\n");
		return 0;
	}
}

int getcommandlen()
{
	int tot_len = 0;
	for (int i = 0 ; i<cmd_cnt;i++)
	{	
		tot_len+=strlen(cmd_array[i]);//spaces not counted in
	}
	return tot_len+cmd_cnt-1;//count in spaces,returns -1 when a single space inputs
}


void printprompt()
{
	char hostname[MAX_NAME_LEN];
	char pathname[MAX_PATH_LEN];
	struct passwd *pwd;
	pwd=getpwuid(getuid());
	gethostname(hostname,MAX_NAME_LEN);
	getcwd(pathname,MAX_PATH_LEN);
	printf("\e[34mmyshell>%s@%s:\e[0m",pwd->pw_name,hostname);
	if (strncmp(pathname,pwd->pw_dir,strlen(pwd->pw_dir))==0)
	{
		printf("~%s",pathname+strlen(pwd->pw_dir));
	}
	else {
		printf("%s",pathname);
	}
	if (geteuid()==0) {
		printf("# ");
	}
	else {
		printf("$ ");
	}
}

void welcome()//weicome screen
{
	
	printf("\e[35m        welcome to myshell\e[0m\n");
	printf("\e[35m        it's a unix-like shell program made by Max.C\e[0m\n");
	printf("\e[35m        hope you enjoy it :)\e[0m\n");
}

void myquit()//friendly quit
{
	printf("Thanks for using my shell!bye-bye!\n");
	sleep(1);
	exit(0);
}

void myexit()//just exit
{

	exit(0);
}

void myclear()
{
	printf("\033[2J");//clear
	printf("\033[H");//move the mouse to a proper positoin
}

//void print_continue_info()
//{
//	printf("myshell: continue: only works in a 'for','while' or 'until' loop\n");
//}

void mypwd()//pwd
{
	char pathname[MAX_PATH_LEN];
	if(getcwd(pathname,MAX_PATH_LEN))//get the path
	{
		printf("%s\n",pathname);
	}
	else//if sth goes wrong
	{
		perror("myshell: getcwd");//report bug
		exit(1);
	}
}
	
void myecho()
{
	for(int i=1;i<cmd_cnt;i++)
	{
		printf("%s",*(cmd_array+i));
		if(i==cmd_cnt-1)//the last string without space
		{
			break;
		}
		printf(" ");
	}
	printf("\n");
}

//void echo_redirect()

void mydir()
{
	char pathname[MAX_PATH_LEN];//save current path
	DIR *dir;
	struct dirent *dp;
	if(!getcwd(pathname,MAX_PATH_LEN))//get path successfully or not
	{
		perror("myshell: getcwd");
		exit(1);
	}
	dir=opendir(pathname);
	//not sorted yet 
	char dname[1000];
	string ntemp[1000];
	int tok=0;
	while((dp=readdir(dir))!=NULL)
	{
		strcpy(dname,dp->d_name);
		ntemp[tok]=dname;
		tok++;
	}
	sort(ntemp,ntemp+tok);
	for(int i=0;i<tok;i++)
	{
		cout<<ntemp[i]<<"\n";
	}
	tok = 0;
}

void myls()
{
	char pathname[MAX_PATH_LEN];//save current path
	DIR *dir;
	struct dirent *dp;
	if(!getcwd(pathname,MAX_PATH_LEN))//get path successfully or not
	{
         perror("myshell: getcwd");
         exit(1);
    }
	dir=opendir(pathname);
	char s[1000];
	string temp[1000];//save file name
	int tok=0;
	while((dp=readdir(dir))!=NULL)
	{
		strcpy(s,dp->d_name);
		if(s[0]!='.')
		{
			temp[tok]=s;
			tok++;
		}
	}
	for(int i=0;i<tok;i++)
	{
		cout<<temp[i]<<"  ";
	}
	printf("\n");
	tok=0;
}

void mycd()
{
	struct passwd *pwd;
	//the passwd structure is defined in <pwd.h> as follows
	//struct passwd{
	//char *pw_name;//user name
	//char *pw_passwd;//user password
	//uid_t pw_uid;//user id
	//gid_t pw_gid;//group id
	//char *pw_gecos;//real name
	//char *pw_dir;//home directory
	//char *pw_shell;//shell program
	char pathname[MAX_PATH_LEN];//save path
	pwd=getpwuid(getuid());//getuid means get user's id
						   //getpwuid means get passwd of this user's id
						   
	if(cmd_cnt==1)//if there's only one cd cmd
	{
		strcpy(pathname,pwd->pw_dir);//get homepath from pw_dir in pwd
		if(chdir(pathname)==-1)//if there's sth goes wrong
		{
			perror("myshell: chdir");
			exit(1);
		}
	}
	else//if there's a directory
	{
		if(chdir(cmd_array[1])==-1)//if something wrong happends in chdir
		{
			printf("myshell: cd: %s :No such file or directory\n",cmd_array[1]);
		}
	}
}//so a single cd makes it back to home directory

void mycat()
{
	if(cmd_cnt==1)
	{
		//print everything input
	}
	else
	{
    	char pathname[MAX_PATH_LEN];//save current path
    	DIR *dir;
    	struct dirent *dp;
    	if(!getcwd(pathname,MAX_PATH_LEN))//get path successfully or not
    	{
    	    perror("myshell: getcwd");
    	    exit(1);
    	}
    	dir=opendir(pathname);
		int amount=cmd_cnt-1;
		int ifexists=0;//check if there's such file or directory
		int flag[amount]={0};//you can only input 3 files at most
	    while((dp=readdir(dir))!=NULL)
		{
			for(int i=1;i<cmd_cnt;i++)
			{
				if(strcmp(cmd_array[i],dp->d_name)==0)//check if the file input exists
				{
					printf("%s:\n",cmd_array[i]);
					fstream file;
					file.open(cmd_array[i]);//open this file.could replace dp->d_name with cmd_array[1]
					char ch=file.get();
					while(ch!=EOF)//check if it's the end of the file
					{
						cout<<ch;
						ch=file.get();
					}
					printf("\n");
					file.close();
					ifexists++;
					flag[i-1]=1;//mark the existing one
				}
			}
		}
		if(ifexists<cmd_cnt-1)//check if there's unexisting one when multiple inputs
		{
			printf("the following files do not exist: ");
			for(int j=0;j<amount;j++)
			{
				if(flag[j]==0)
				{
					printf("%s  ",cmd_array[j+1]);//output not existing ones
				}
			}
			printf("\n");
		}
	}
}

void mymkdir()
{
	if(cmd_cnt>2)
	{
		cout<<"myshell: too many arguments!\n";
	}
	else 
		if(mkdir(cmd_array[1],0744)==-1)
		{
			cout<<"myshell: create dir failed!";
		}
}

void mywordcount()
{
	if(cmd_cnt==1)
	{
		cout<<"myshell: what do you wanna know?";
	}
	else
	{
		char pathname[MAX_PATH_LEN];
		DIR *dir;
		struct dirent *dp;
		int amount=cmd_cnt-1,count=0,ifexists=0;
		int flag[amount]={0};
		if(!getcwd(pathname,MAX_PATH_LEN))
		{
			perror("myshell: getcwd");
			exit(1);
		}
		dir=opendir(pathname);
		while((dp=readdir(dir))!=NULL)
		{
			for(int i=1;i<cmd_cnt;i++)
			{
				if(strcmp(cmd_array[i],dp->d_name)==0)
				{
					fstream file;
					file.open(cmd_array[i]);
					char ch=file.get();
					while(ch!=EOF)
					{	
						count++;
						ch=file.get();
					}
					file.close();
					printf("%d characters in file : %s\n",count,cmd_array[i]);
					count = 0;
					ifexists++;
					flag[i-1]=1;
				}
			}
		}
		if(ifexists<cmd_cnt-1)
		{
			printf("the following files do not exist: ");
			for(int j=0;j<amount;j++)
			{
				if(flag[j]==0)
				{
					printf("%s  ",cmd_array[j+1]);
				}
			}
			printf("\n");
		}
	}
}	






void mybatch()
{

}

void mytime()
{

}

 














int main()
{
	int should_run=1;//when to end the outest loop
	pid_t pid;//when to use fork
	int cmd_len=0;//record the length of cmd
	//int pos_after_pipe;//the position of pipe
	//int bg;//the mark of bg
	welcome();
	while(should_run)
	{
		printprompt();
		readcommand();
		cmd_len=getcommandlen();
		if(cmd_len>MAX_LINE)
		{
			printf("your command is too long!");
			exit(1);
		}
		if(is_internal_cmd())
		{
			continue;
		}
	}
    return 0;
}


























