#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
struct CommandLine {
	char *word;
	struct CommandLine *next;
};
enum {
	chunk_size = 8
};
struct String {
	int x[chunk_size]; //elements of chunk
	struct String *next;
};

/* questions

errors in procedure? maybe
memory clean?

*/

struct String *StringFill()
{
	struct String *str,*temp; //we use it when we create another buffer
	int i=0;
	int c; //our char with EOF
	str = malloc(sizeof(*str));
	temp = str;
	while((c = getchar()) != '\n')//EOF)
	{
		if (i == chunk_size)
		{
			temp->next=malloc(sizeof(*str));
			temp=temp->next;
			i = 0;
		}
		temp->x[i]=c;
		i++;
	}
	//if (c == EOF)
	if (i != chunk_size)
	{
		temp->x[i]=EOF;
	}
	temp->next=NULL;
	/* is this free memory? */
	temp = NULL;

	return str;
}

void StringPrint(const struct String *str)
{
	const struct String *temp; //what it does?
	int i=0;
	int c; //our char with EOF
	temp = str;
	while(temp!=NULL)
	{
		while((c=(temp->x[i]))!=EOF && (i<chunk_size)) 
		{
			putchar(c);
			i++;
		}
		temp=temp->next;
		i=0;
	}
}

void StringFree(struct String *str) //we need test for this thing
{
	if (str != NULL)
	{
		StringFree(str->next);
		free(str);
	}
}

int StringSize(const struct String *str)
{
	int n=0, i=0,c;
	const struct String *temp;
	temp = str;
	while(temp!=NULL)
	{
		while((c=(temp->x[i]))!=EOF && (i<chunk_size)) 
		{
			n++; i++;
		}
		temp=temp->next;
		i=0;
	}
	return n;
}

/*we need some procedures to separate string into words*/
struct String *StringMakeNewFrom(struct String *str,int s1,int s2)
{
	struct String *temp, *newstr, *newtemp;
	int i,j,l;
	temp = str;
	s1 -= 1; s2 -= 1; /*shift*/
	s2 = s2 - s1; //pass few lines we'll make another base
	//move to our 1st separator
	for (i=0;i<(s1/chunk_size);i++)
	{
		temp = temp->next;
	}
	s1 = s1 % chunk_size; //we made another base
	s2 = s1 + s2; //s2 points in new base
	newstr = malloc(sizeof(*newstr));
	newtemp = newstr;
	for (j=s1;j<=s2;j++)
	{
		l = j-s1; //index in chunk of newstr
		if ( (j != 0) && ((j)%chunk_size) == 0)
		{
			temp=temp->next;
		}
		if ( (l != 0) && ((l)%chunk_size) == 0)
		{
			newtemp->next=malloc(sizeof(*newstr));
			newtemp=newtemp->next;
		}
		newtemp->x[(l%chunk_size)]=temp->x[(j%chunk_size)];
	}
	l = j-s1; //index in chunk of newstr
	//ending
	if ( ((l)%chunk_size) != 0 )
	{
		newtemp->x[(l%chunk_size)]=EOF;
	}
	newtemp->next=NULL;	
	/* is this free memory? */
	newtemp = NULL;

	return newstr;
}

char StringCharAt(const struct String *str, int i)
{
	i = i - 1;
	int j;
	for (j=0;j<(i/chunk_size);j++)
	{
		str=str->next;
	}
	return str->x[i%chunk_size];
}

char *StringMakeRealStringFrom(struct String *str, int s1, int s2)
{ /*we add it to dont overwrite all of this*/
	struct String *temp;	
	char *p;
	temp = StringMakeNewFrom(str,s1,s2);
	int i, size = StringSize(temp);
	p = malloc((size+1));
	for (i=1;i<=size;i++)
	{
		p[i-1] =(char)StringCharAt(temp,i); 
	}
	p[i-1] = 0;
	/* is this free memory? */
	StringFree(temp);
	temp = NULL;
	
	return p;
}

int RealStringEqual(char *s1, char *s2)
{
	int i=0;
	while (s1[i] || s2[i])
	{
		if (s1[i] != s2[i])
		{
			return 0;
		}
		i+=1;
	}
	return 1;
}

void CommandLinePrint(const struct CommandLine *line)
{
	while (line!=NULL)
	{
		printf("%s",line->word);
		//printf("\t\t\t size:%lu",sizeof(line->word));
		printf("\n");
		line=line->next;
	}
}

int CommandLineSize(const struct CommandLine *line)
{
	int i=0;
	while (line != NULL)
	{
		line = line->next; i++;
	}
	return i;
}

void CommandLineFree(struct CommandLine *line)
{
	if (line != NULL)
	{
		CommandLineFree(line->next);
		free(line->word);
		free(line);
	}
}

struct CommandLine *CommandLineAddWord(struct CommandLine *line, struct String *str, int s1, int s2)
{ //we can use recursion, it will be more compact code here with it
	struct CommandLine *linetemp, *lineprev;
	int first = 1;
	linetemp = line;
	if (s1<=s2)
	{
		while(linetemp!=NULL)
		{
			lineprev = linetemp;
			linetemp = linetemp->next;
			first = 0;
		}
		if (first)
		{
			linetemp = malloc(sizeof(*line));
			linetemp->word = StringMakeRealStringFrom(str,s1,s2);
			linetemp->next = NULL;
			line = linetemp;
		} else {
			lineprev->next = malloc(sizeof(*line));
			linetemp = lineprev->next;
			linetemp->word = StringMakeRealStringFrom(str,s1,s2);
			linetemp->next = NULL;
		}
	} /*else => error*/
	
	/* is this free memory? */
	linetemp = NULL;
	lineprev = NULL;
	
	return line;
}

struct CommandLine *CommandLineFromString(struct String *str)
{
	struct CommandLine *line;
	//struct String *empty; //possible desicion for s1>s2
	line = NULL;
	int i, size;
	//int first = 0;
	int s1,s2; //our separators
	//we're moving through our string, if ' ' or '"' => separate
	i=1; //we point at first symbol
	size = StringSize(str);
	while(i<=size)
	{
		if (StringCharAt(str,i)=='"')
		{
			i = i + 1;
			//if i > size => error: unbalanced "
			s1 = i; //save our 1st separator
			while(i<=size && (StringCharAt(str,i)!='"'))
			{
				i += 1;
			}
	//???errors in procedure???
			if (i > size)
			{
				printf("error:: unbalanced commas\n");
				break;
			}
			s2 = i - 1; //save our 2d separator
			if (s1 > s2)
			{
				printf("error:: empty commas\n");
				break;
			}
			line = CommandLineAddWord(line,str,s1,s2);
			i = i + 1;
		} else
			if (StringCharAt(str,i)!=' ') 
			{
			s1 = i;
			i += 1;
			while(i<=size && (StringCharAt(str,i)!=' '))
			{
				i += 1;
			}
			s2 = i - 1;
		//!!!if char = "\n" ->
			if (StringCharAt(str,s2) == '\n')
			{
				s2 = s2 - 1;
			}
		//!!! <-
			line = CommandLineAddWord(line,str,s1,s2);
		} else {
			i += 1;
		}
		//printf("I SEE CHAR = (%c)\n",StringCharAt(str,i));
	}
	//!!!!!!
	if (size == 0)
	{
		struct String *str0;
		str0 = malloc(sizeof(str0));
		str0->x[0] = 0;
		str0->x[1] = EOF;
		line = CommandLineAddWord(line,str0,1,1);
		
	}
	
	return line;
}

char **CommandLineConverter(const struct CommandLine *line)
{ /*our |line| became |cline|*/
	int i, size = CommandLineSize(line);
	char **p;
	p = malloc((size+1)*sizeof(*p));
	for (i=0;i<size;i++)
	{
		p[i] = line->word;
		line = line->next;
	}
	p[i] = NULL;
	return p;
}

void ClinePrint(char * const *cline)
{
	int i=0;
	while (cline[i] != NULL)
	{
		printf("%s\n",cline[i]); i++;
	}
}

void ClineFree(char ** cline)
{
	/*
	int i=0;
	while(cline != NULL)
	{
		free(cline[i]); i++;
	}
	*/
	free(cline);
}

int CommandCD(const struct CommandLine *line)
{
	if (RealStringEqual(line->word,"cd"))
	{
		return 1;
	} else {
		return 0;
	}
}

void CommandLineProcessor(const struct CommandLine *line)
{ 
	int r,p;
	char **cline = CommandLineConverter(line);
	if (CommandCD(line))
	{
		r = chdir(cline[1]);
		if (r == -1)
		{
			perror(cline[1]);
		}
	} else {
		p = fork();
		if (p == 0)
		{
			//printf("cline[0] = [%s]\n",cline[0]);
			if (!RealStringEqual(cline[0],""))
			{
				execvp(cline[0],cline);
				perror(cline[0]);
				fflush(stderr);
				exit(1);
			}
		}
		wait(NULL);
	}
	ClineFree(cline);
}


int main()
{
	
	struct CommandLine *line;
	struct String *str;
	
	for(;;)
	{
		printf("-->");
		str = StringFill();
		
		line = CommandLineFromString(str);
		
		/*
		char **cline;
		cline = CommandLineConverter(line);
		printf("your line\n");
		CommandLinePrint(line);
		printf("your cline\n");
		ClinePrint(cline);
		*/
		/*!!!problem with empty string*/
		CommandLineProcessor(line);
		
		StringFree(str);
		CommandLineFree(line);
		str = NULL; line = NULL;	
	}
			
	
	
	
}
