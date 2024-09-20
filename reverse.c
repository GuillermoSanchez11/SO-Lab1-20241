#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Line
{
    char *text;
    struct Line *next;
} Line;

Line *create_line(const char *text)
{
    Line *line = (Line *)malloc(sizeof(Line));
    if (!line)
    {
        fprintf(stderr, "Error: No se pudo asignar memoria\n");
        exit(1);
    }
    line->text = strdup(text);
    if (!line->text)
    {
        fprintf(stderr, "Error: No se pudo asignar memoria para el texto\n");
        free(line);
        exit(1);
    }
    line->next = NULL;
    return line;
}

Line *read_lines_from_file(FILE *file)
{
    Line *head = NULL, *tail = NULL;
    char *buffer = NULL;
    size_t len = 0;
    ssize_t read;

    if (!file)
        return NULL;

    while ((read = getline(&buffer, &len, file)) != -1)
    {
        // If buffer does not end with a newline, add it
        if (read > 0 && buffer[read - 1] != '\n')
        {
            buffer[read] = '\n';
            buffer[read + 1] = '\0';
        }

        Line *new_line = create_line(buffer);
        if (!head)
        {
            head = new_line;
        }
        else
        {
            tail->next = new_line;
        }
        tail = new_line;
    }

    free(buffer);
    return head;
}

Line *read_lines_from_stdin()
{
    return read_lines_from_file(stdin);
}

void write_lines_to_file(const char *filename, Line *head)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        fprintf(stderr, "Error: No se pudo abrir el archivo %s\n", filename);
        exit(1);
    }

    Line *current = head;
    while (current)
    {
        fprintf(file, "%s", current->text);
        current = current->next;
    }

    fclose(file);
}

Line *reverse_lines(Line *head)
{
    Line *prev = NULL, *current = head, *next = NULL;
    while (current)
    {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }

    return prev;
}

void free_lines(Line *head)
{
    Line *temp;
    while (head)
    {
        temp = head;
        head = head->next;
        free(temp->text);
        free(temp);
    }
}

const char *get_file_name(const char *path)
{
    const char *last_slash = strrchr(path, '/'); // Find the last '/'
    if (last_slash != NULL)
    {
        return last_slash + 1; // Return the part after the last '/'
    }
    return path; // If no '/', return the full path (it's already a file name)
}

int are_file_names_equal(const char *path1, const char *path2)
{
    const char *file_name1 = get_file_name(path1);
    const char *file_name2 = get_file_name(path2);

    return strcmp(file_name1, file_name2) == 0;
}

int main(int argc, char *argv[])
{
    Line *lines = NULL;

    if (argc == 1)
    {
        lines = read_lines_from_stdin();
        if (lines == NULL)
        {
            exit(1);
        }
        lines = reverse_lines(lines);
        Line *current = lines;
        while (current)
        {
            printf("%s", current->text);
            current = current->next;
        }
    }
    else if (argc == 2)
    {
        FILE *file = fopen(argv[1], "r");
        if (!file)
        {
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]);
            exit(1);
        }
        lines = read_lines_from_file(file);
        fclose(file);

        if (lines == NULL)
        {
            exit(1);
        }
        lines = reverse_lines(lines);
        Line *current = lines;
        while (current)
        {
            printf("%s", current->text);
            current = current->next;
        }
    }
    else if (argc == 3)
    {
        if (strcmp(argv[1], argv[2]) == 0)
        {
            fprintf(stderr, "reverse: input and output file must differ\n");
            exit(1);
        }
        if (are_file_names_equal(argv[1], argv[2]))
        {
            fprintf(stderr, "reverse: input and output file must differ\n");
            exit(1);
        }
        FILE *input_file = fopen(argv[1], "r");
        if (!input_file)
        {
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]);
            exit(1);
        }
        lines = read_lines_from_file(input_file);
        fclose(input_file);

        if (lines == NULL)
        {
            exit(1);
        }
        lines = reverse_lines(lines);
        write_lines_to_file(argv[2], lines);
    }
    else
    {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }

    free_lines(lines);
    return 0;
}