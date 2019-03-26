int main(void)
{
    char *p, s[100];



    scanf("%s", s);
    p = s;

    while(*p) {
	if (*p >= *"a" && *p <= *"z")
	    *p = *p + *"A" - *"a";
	p = p + 1;
    }

    printf("%s\n", s);
}
