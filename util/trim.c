    /*去除字符串中所有空格*/
voidVS_StrTrim(char*pStr)
{
    char *pTmp = pStr;
    while (*pStr != '/0')
    {
    if (*pStr != ' ')
    {
    *pTmp++ = *pStr;
    }
    ++pStr;
    }
    *pTmp = '/0';
}

    /*去除字符串右边空格*/   
void VS_StrRTrim(char *pStr)
{
    char *pTmp = pStr+strlen(pStr)-1;
    while (*pTmp == ' ')
    {
    *pTmp = '/0';
    pTmp--;
    }
}
    /*去除字符串左边空格*/
 void VS_StrLTrim(char *pStr)
{
    char *pTmp = pStr;
    while (*pTmp == ' ')
    {
    pTmp++;
    }
    while(*pTmp != '/0')
    {
    *pStr = *pTmp;
    pStr++;
    pTmp++;
    }
    *pStr = '/0';
} 