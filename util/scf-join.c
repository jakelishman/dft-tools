#include <stdio.h>
#include <stdlib.h>

#include <stdbool.h>

#define LINEBUF 4096

struct data {
    double vol;
    double cbya;
    double mag;
    double ef;
    double etot;
    bool eof;
};

enum point_to_write {
    POINT_NONE,
    POINT_BOTH,
    POINT_0,
    POINT_1
};

struct data next_point (FILE *fp);
enum point_to_write point_to_write (struct data *points);

int main (int argc, char **argv)
{
    if (argc != 3) {
        fprintf (stderr, "Argument error\n");
        return EXIT_FAILURE;
    }

    FILE *in[2] = {fopen (argv[1], "r"), fopen (argv[2], "r")};
    if (!in[0] || !in[1]) {
        fprintf (stderr, "File error\n");
        return EXIT_FAILURE;
    }

    int ret = EXIT_SUCCESS;
    struct data points[2];
    bool get_next[2] = {true, true};

    while (get_next[0] || get_next[1]) {
        if (get_next[0])
            points[0] = next_point (in[0]);

        if (get_next[1])
            points[1] = next_point (in[1]);

        switch (point_to_write (points)) {
        case POINT_BOTH:
            printf ("%.15g,%.15g,%.15g,%.15g,%.15g,%.15g,%.15g,%.15g\n",
                    points[0].vol, points[0].cbya,
                    points[0].mag, points[0].ef, points[0].etot,
                    points[1].mag, points[1].ef, points[1].etot);
            get_next[0] = get_next[1] = true;
            break;
        case POINT_0:
            printf ("%.15g,%.15g,%.15g,%.15g,%.15g,,,\n",
                    points[0].vol, points[0].cbya,
                    points[0].mag, points[0].ef, points[0].etot);
            get_next[0] = true;
            get_next[1] = false;
            break;
        case POINT_1:
            printf ("%.15g,%.15g,,,,%.15g,%.15g,%.15g\n",
                    points[1].vol, points[1].cbya,
                    points[1].mag, points[1].ef, points[1].etot);
            get_next[0] = false;
            get_next[1] = true;
            break;
        case POINT_NONE:
            get_next[0] = get_next[1] = false;
            break;
        }
    }

    fclose (in[0]);
    fclose (in[1]);

    return ret;
}

struct data next_point (FILE *fp)
{
    char buf[LINEBUF];
    struct data out;
    while (fgets (buf, LINEBUF, fp)) {
        if (buf[0] == '#' || buf[0] == '\n')
            continue;

        sscanf (buf, "%lf, %lf, %lf, %lf, %lf\n", &out.vol, &out.cbya, &out.mag, &out.ef, &out.etot);
        out.eof = false;
        return out;
    }

    return (struct data) {.eof = true};
}

enum point_to_write
point_to_write (struct data points[2])
{
    if (points[0].eof && points[1].eof) return POINT_NONE;
    if (points[0].eof) return POINT_1;
    if (points[1].eof) return POINT_0;

    if (points[0].vol < points[1].vol) return POINT_0;
    if (points[1].vol < points[0].vol) return POINT_1;

    if (points[0].cbya < points[1].cbya) return POINT_0;
    if (points[1].cbya < points[0].cbya) return POINT_1;

    return POINT_BOTH;
}
