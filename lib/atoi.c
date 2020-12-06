int atoi(const char *nptr) {
    int ans = 0;
    char c;
    while (c = *nptr) {
        if (c < '0' || c > '9') {
            break;
        }
        ans = ans * 10 + (c - '0');
        nptr++;
    }
    return ans;
}
