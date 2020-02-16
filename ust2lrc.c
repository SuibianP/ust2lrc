#include <stdio.h>
#include <stdlib.h>  //for exit() and memory allocation
#include <string.h>  //for string functions
#include <ctype.h>   //for isdigit()
#include <stdbool.h> //for boolean

#define MAX_UST_LINE 1024
#define MAX_UST_LYRIC 512
#define MAX_UST_PROJECTNAME 512
#define MAX_LRC_LYRIC 512
#define UST2LRC_SYNTAX "Usage syntax: ust2lrc <ust file> [[-e|-s] [<delimiters> [<threshold>]]]\nExample: ust2lrc source.ust -e R,r 240\nYou may want to use quotes or escape characters to pass an argument containing white character(s)."

//Usage syntax: ust2lrc <ust file> [[-e|-s] [<delimiters> [<threshold>]]]
//Example: ust2lrc source.ust R,r 240
//You may want to use quotes or escape characters to pass an argument containing space.
//TODO: encoding convert, input validation

struct ustline
{
    int Length; //in ticks
    char Lyric[MAX_UST_LYRIC];
    float Tempo;
};

bool isDelimiter(char Delimiter[], char Source[])
{
    char *delimpt = strdup(Delimiter); //since strtok() modifies the passed string
    char *tokpt = strtok(delimpt, ",");
    if (!strcmp(Source, tokpt))
        return true;
    while (tokpt != NULL)
    {
        if (!strcmp(Source, tokpt))
            return true;
        tokpt = strtok(NULL, ",");
    }
    return false;
}

int main(int argc, char *argv[])
{
    bool isSimple;
    char Delimiter[MAX_UST_LYRIC];
    int DelimiterLengthThreshold = 0;
    if (argc < 2 || argc > 5)
    {
        puts(UST2LRC_SYNTAX);
        exit(0);
    }
    else if (argc == 2) //interactive mode
    {
        fputs("Simple lrc/Enhanced lrc? (S/E) ", stdout); //not to print \n
        switch (getchar())
        {
        case 'B':
            isSimple = true;
            break;
        case 'A':
            isSimple = false;
            break;
        default:
            puts("Invalid input.");
            puts(UST2LRC_SYNTAX);
            exit(0);
        }
        if (!isSimple)
        {
            fputs("Input all delimiters, separated by commas. ", stdout);
            scanf("%s", Delimiter);
            fputs("Input delimiter length threshold in ticks (integer), 0 for no threshold. ", stdout);
            scanf("%d", &DelimiterLengthThreshold);
        }
    }
    else //argument mode
    {
        //handle arg 3 [-e|-s]
        if (strcmp(argv[2], "-s"))
            isSimple = true;
        else if (strcmp(argv[2], "-e"))
            isSimple = false;
        else
        {
            puts("Invalid input.");
            puts(UST2LRC_SYNTAX);
            exit(0);
        }
        //handle arg 4 delimiter
        if (argc > 3)
        {
            strcpy(Delimiter, argv[3]);
            //handle arg 5 threshold
            if (argc > 4)
                DelimiterLengthThreshold = atoi(argv[4]);
        }
    }

    char tempustline[MAX_UST_LINE], ProjectName[MAX_UST_PROJECTNAME];
    int count = 0, i; //count for note count, i for loop
    FILE *inust = fopen(argv[1], "r");
    if (inust == NULL)
    {
        perror("\nFile access failure");
        exit(0);
    }
    struct ustline *ustinstance = malloc(sizeof(struct ustline));
    float CurrentTempo;
    double CumulativeTime = 0.0;
    while (fgets(tempustline, MAX_UST_LINE, inust) != NULL)
    {
        if (!strncmp(tempustline, "[#", 2) && isdigit(tempustline[3])) //if new note section
        {
            if (ustinstance[count - 1].Tempo == -1)
                ustinstance[count - 1].Tempo = CurrentTempo; //when Tempo entry in not present
            ustinstance = realloc(ustinstance, (++count) * sizeof(struct ustline));
            //initialize newly allocated structure elements
            ustinstance[count - 1].Length = 0;
            ustinstance[count - 1].Lyric[0] = '\0';
            ustinstance[count - 1].Tempo = -1;
        }
        else if (sscanf(tempustline, "Length=%d", &ustinstance[count - 1].Length))
            ;
        else if (sscanf(tempustline, "Lyric=%[^\n]", ustinstance[count - 1].Lyric))
            ;
        else if (sscanf(tempustline, "Tempo=%f", &CurrentTempo))
            ustinstance[count ? count - 1 : count].Tempo = CurrentTempo; //for the first Tempo entry in [#SETTING] section
        else if (sscanf(tempustline, "ProjectName=%[^\n]", ProjectName))
            ;
    }
    if (ustinstance[count - 1].Tempo == -1) //this is not elegant but has to be done to the last note
        ustinstance[count - 1].Tempo = CurrentTempo;
    fclose(inust);
    //input into array complete

    FILE *outlrc = fopen("out.lrc", "w");
    if (outlrc == NULL)
    {
        perror("\nFile access failure");
        exit(0);
    }
    //basic lrc output
    if (isSimple)
    {
        fprintf(outlrc, "[ti=%s]\n[re=ust2lrc by Suibian]\n", ProjectName);
        for (i = 0; i < count; ++i)
        {
            if (!strcasecmp(ustinstance[i].Lyric, "R")) //if rest
                ustinstance[i].Lyric[0] = '\0';
            fprintf(outlrc, "[%02d:%02d.%02d]%s\n", (int)CumulativeTime / 100 / 60, (int)CumulativeTime / 100 % 60, (int)CumulativeTime % 100, ustinstance[i].Lyric);
            CumulativeTime += 12.5 * ustinstance[i].Length / ustinstance[i].Tempo;
        }
    }
    //enhanced lrc output
    else
    {
        fprintf(outlrc, "[ti=%s]\n[re=ust2lrc by Suibian]\n", ProjectName);
        for (i = 0; i < count; ++i)
        {
            if (i == 0 && !isDelimiter(Delimiter, ustinstance[0].Lyric))
                fputs("[00:00.00]", outlrc);
            if (i)
                fprintf(outlrc, "<%02d:%02d.%02d>", (int)CumulativeTime / 100 / 60, (int)CumulativeTime / 100 % 60, (int)CumulativeTime % 100);
            if (!(isDelimiter(Delimiter, ustinstance[i].Lyric) && ustinstance[i].Length > DelimiterLengthThreshold)) //not an ignored delimiter
                fprintf(outlrc, "%s", ustinstance[i].Lyric);
            CumulativeTime += 12.5 * ustinstance[i].Length / ustinstance[i].Tempo;
            if (isDelimiter(Delimiter, ustinstance[i].Lyric) && i != count - 1) //delimiter but not track end, or track start
                fprintf(outlrc, "\n[%02d:%02d.%02d]", (int)CumulativeTime / 100 / 60, (int)CumulativeTime / 100 % 60, (int)CumulativeTime % 100);
        }
    }
    fclose(outlrc);
    free(ustinstance);
    return 0;
}
