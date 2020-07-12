#ifndef _SEARCH_H_HEADER  
#define _SEARCH_H_HEADER  
  
#define TEXT_SZ 2048  
  
struct shared_use_st  
{  
    int written;	//×÷ÎªÒ»žö±êÖŸ£¬·Ç0£º±íÊŸ¿É¶Á£¬0±íÊŸ¿ÉÐŽ
    char text[TEXT_SZ];  //ŒÇÂŒÐŽÈëºÍ¶ÁÈ¡µÄÎÄ±Ÿ	
};  
  

int movefile() ;

#endif
