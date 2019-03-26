int main(void)
{
    double *p;

    p = (double *) malloc(sizeof(double));
    *p = 1.23;
    printf("%f\n", *p);
}
