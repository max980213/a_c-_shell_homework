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
//Header files might be used

using namespace std;

#define MAX_LINE 80//longest cmd
#define MAX_NAME_LEN 100//longest username
#define MAX_PATH_LEN 1000//longest path

//declare global variabless
char *cmd_array[MAX_LINE/2+1];//inputing cmd
int cmd_cnt;//the number of strings in each cmd

//declare functions
//basic functions
void readcommand();//read cmd
int is_internal_cmd();//internal cmd

//self defined functions
void weicome();//welcome screen
void printprompt();//print cmd with path
int getcommandlen();//calculate the length of each cmd
void myquit();//just quit this shell
void myexit();//exit directly
void myclear();//like clear in bash
void mypwd();//like pwd in bash
void myecho();//like echo in bash
void mytime();//like time in bash
void mycd();//like cd in bash
void myhelp();//iike..help
void print_manual();//print user nanual,the son of help()
void mybatch();//sth like bat
void mydir();
void myls();//like ls in bash
void mycat();
void mymkdir();
void mywordcount();//print the number of charactors of files
void myrm();//
void myrmdir();//
void mytime();//
void mybatch();//i dont wanna do this
void myll();
void mypanel();
void myhelp();

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
	else if(strcmp(cmd_array[0],"dir")==0)
	{
		mydir();
		//return 1;
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
	else if(strcmp(cmd_array[0],"help")==0)
	{
		mypanel();
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
	else if(strcmp(cmd_array[0],"rm")==0)
	{
		myrm();
		return 1;
	}
	else if(strcmp(cmd_array[0],"rmdir")==0)
	{	
		myrmdir();
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
	printf("myshell>\e[032m%s@%s:\e[0m",pwd->pw_name,hostname);
	if (strncmp(pathname,pwd->pw_dir,strlen(pwd->pw_dir))==0)
	{
		printf("\e[036m~%s\e[0m",pathname+strlen(pwd->pw_dir));
	}
	else {
		printf("\e[036m%s\e[0m",pathname);
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
	printf("\e[33mThanks for using my shell!bye-bye!\e[0m\n");
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
	
void myecho()//如果输入太多字符会报错
{	
	char *variable = NULL;
	char environment[MAX_PATH_LEN];
	for(int i=1;i<cmd_cnt;i++)
	{
		variable = cmd_array[i];
		if(variable[0] == '$')
		{
			strcpy(environment,variable+1);
			char *buff=getenv(environment);
			if(buff)
			{
				cout<<buff<<endl;
			}
			else
			{
				cout<<endl;
			}
		}
		else
		{
			printf("%s\n",*(cmd_array+i));
		}
	}
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
	struct stat s;
	char pathname[MAX_PATH_LEN];//save current path
	DIR *dir;
	struct dirent *dp;
	if(!getcwd(pathname,MAX_PATH_LEN))//get path successfully or not
	{
         perror("myshell: getcwd");
         exit(1);
    }
	dir=opendir(pathname);
	char temp[1000];
	while((dp=readdir(dir))!=NULL)
	{
		strcpy(temp,dp->d_name);
		if(temp[0]!='.')
		{
			if(stat(dp->d_name,&s)==0)
			{
				if(s.st_mode&S_IFDIR)
				{
					printf("\e[34m%s  \e[0m",dp->d_name);
				}
				else
				{
					printf("%s  ",dp->d_name);
				}
			}
		}
	}
	cout<<endl;
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
  	else if(cmd_cnt>2)
	{
		cout<<"myshell: cd: what are you,ninja?"<<endl;
	}
	else//if there's a directory
	{
		if(chdir(cmd_array[1])==-1)//if something wrong happends in chdir
		{
			perror("myshell: cd: ");
		}
	}
}//so a single cd makes it back to home directory

void mycat()
{	
	if(cmd_cnt==1)//only one single input
    {
        cout<<"myshell: cat: please input at least one file!\n";
    }
	else
	{
		struct stat s;
		char buff;
		int flag[cmd_cnt-1]={0},ifexists=0;
		int frd;//file open index
    	for(int i=1;i<cmd_cnt;i++)
		{
			if(stat(cmd_array[i],&s)==0)
			{
				if(s.st_mode&S_IFDIR)
				{
					flag[i-1]=2;
					continue;
				}
				else
				{
					if((frd=open(cmd_array[i],O_RDONLY))!=-1)
					{
						printf("%s:\n",cmd_array[i]);
						while(read(frd,&buff,sizeof(char))!=0)
						{
							printf("%c",buff);//print characters
						}
						printf("\n");
						flag[i-1]=1;
						ifexists++;
					}
				}	
			}
		}		
		if(ifexists<cmd_cnt-1)//check if there's unexisting one when multiple inputs
		{
			for(int j=0;j<cmd_cnt-1;j++)
			{
				if(flag[j]==0)
				{
					cout<<"myshell: cat: file "<<cmd_array[j+1]<<" does not exist"<<endl;
				}
				if(flag[j]==2)
				{
					cout<<"myshell: cat: "<<cmd_array[j+1]<<" is a directory"<<endl;
				}	
			}
		}
	}
}

void mymkdir()
{
	for(int i=1;i<cmd_cnt;i++)
	{
		if(mkdir(cmd_array[i],0744)==-1)
		{
			perror("myshell: mkdir: ");
		}
	}
}

void mywordcount()
{
	if(cmd_cnt==1)
	{
		cout<<"myshell: wc: what do you wanna know?\n";
	}
	else
	{
		struct stat s;
		char buff;
		int flag[cmd_cnt-1]={0},ifexists=0,count=0;
		int frd;
		for(int i=1;i<cmd_cnt;i++)
		{
			if(stat(cmd_array[i],&s)!=0)
			{	
				cout<<"myshell: wc: file "<<cmd_array[i]<<" does not exist"<<endl;
				continue;
			}
			else
			{
				if(s.st_mode&S_IFDIR)
				{
					cout<<"myshell: wc: "<<cmd_array[i]<<" is a directory"<<endl;
				}
				else	
				{
					if((frd=open(cmd_array[i],O_RDONLY))!=-1)
					{
						while(read(frd,&buff,sizeof(char))!=0)
						{	
							count++;
						}
						cout<<"myshell: wc: "<<count<<" characters in "<<cmd_array[i]<<endl;
					}
				}	
			}
		}
	}
}


void myrm()
{
	for(int i=1;i<cmd_cnt;i++)
	{
		if(remove(cmd_array[i])!=0)
		{
			perror("myshell: rm ");
		}
	}
}

void myrmdir()
{
	for(int i=1;i<cmd_cnt;i++)
	{
		if((rmdir(cmd_array[i]))!=0)
		{
			perror("myshell: rmdir ");
		}
	}
}	

void mytime()
{
	int weekday;
	int month;
	time_t tvar;
	struct tm *tp;
	time(&tvar);//time_t time(time_t *t)
	tp=localtime(&tvar);//get local time    struct tm *localtime(const time_t *timer)
	weekday=tp->tm_wday;
	switch(weekday)
	{
		case 1:
			cout<<"Mon ";
			break;
		case 2:
			cout<<"Tue ";
			break;
		case 3:
			cout<<"Wed ";
			break;
		case 4:
			cout<<"Thu ";
			break;
		case 5:
			cout<<"Fri ";
			break;
		case 6:
			cout<<"Sat ";
			break;
		case 7:
			cout<<"Sun ";
			break;
	}
	month=1+tp->tm_mon;//the month in the structs begin with 0
	switch(month)
	{
		case 1:
			printf("Jan ");
			break;
		case 2:
			printf("Feb ");
			break;
		case 3:
			printf("Mar ");
			break;
		case 4:
			printf("Apr ");
			break;
		case 5:
			printf("May ");
			break;
		case 6:
			printf("Jun ");
			break;
		case 7:
			printf("Jul ");
			break;
		case 8:
			printf("Aug ");
			break;
		case 9:
			printf("Sep ");
			break;
		case 10:
			printf("Oct ");
			break;
		case 11:
			printf("Nov ");
			break;
		case 12:
			printf("Dec ");
			break;
	}
	printf("%d ",tp->tm_mday);
	printf("%d:",tp->tm_hour);
	printf("%d:",tp->tm_min);
	printf("%d ",tp->tm_sec);
	printf("CST ");
	printf("%d\n",1900+tp->tm_year);
}

void mypanel()
{
	printf("welcome to the panel of myshell, hope this works for you\n");
	printf("\n");
	printf("NAMES      FORMATS                         DESCRIPTIONS\n");
	printf("cd:        cd [dir]                        go to a specified directory\n");
	printf("echo:      echo [args] or $[arg]           print strings after echo,environment variables avaliable\n");
	printf("exit:      exit                            quit the shell directly\n");
	printf("pwd:       pwd                             print the current working directory\n");
	printf("time:      time                            show the current time in an elegant format\n");
	printf("clear:     clear                           clear the screen\n");
	printf("dir:       dir [dir]                       list the file names in the target directory\n");
	printf("ls:        ls                              list the files\n");
	printf("help:      help                            show the panel\n");
	printf("bye:       bye                             quit the shell with thank-you information\n");
	printf("mkdir:     mkdir [filename]                create a directory in current directory\n");
	printf("cat:       cat [filename]                  show contents of files\n");
	printf("wc:        wc [filename]                   show the number of characters\n");
	printf("rm:        rm [file || directory name]     remove it\n");
	printf("rmdir:     rmdir [directory]         	   remove it\n");
	
	fflush(stdout);
}

void mybatch()
{
	cout<<"I haven't and don't want to and won't do this part.."<<endl;
}


int main()
{
	int should_run=1;//when to end the outest loop
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




