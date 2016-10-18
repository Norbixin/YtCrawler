#include <bits/stdc++.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include <sstream>

#define maxrange 10 // how many url's are considered for the next video
#define maxtime 20 // maximum length of video in minutes

struct slink{
    int viewCount;
    std::string videoUrl;
    std::string videoName;
    std::string videoTime;
};
const char *url;
std::map<std::string, bool> visited;
std::string urlstr="";
std::string input, match="href=\"/watch", match2="le=\"", match3="w-count\">", match4="ime\">", prefix="https://www.youtube.com/watch";
std::vector<slink> links;
std::ostringstream stream;
unsigned int n;

bool sortOperator(slink a, slink b){
    return a.viewCount<b.viewCount;
}

// Function for Curl write
size_t write_data(char *ptr, size_t size, size_t nmemb, void *userdata) {
    std::ostringstream *stream = (std::ostringstream*)userdata;
    size_t count = size * nmemb;
    stream->write(ptr, count);
    return count;
}

// Random
int roll(int x) {
    return rand()%(x+1);
}

// Search for url's
void search(std::string str){
    links.clear();
    std::string newlink, size, name, vTime;
    for( int i=0; i<str.length(); ++i ){
        if( str[i]=='h' ){
            bool create=false;
            newlink=prefix;
            size=name=vTime="";
            int j=0;
            for( ; j<match.size(); ++j ){
                if( str[i+j]==match[j] ){
                    continue;
                }
                else
                    break;
            }
            // Make link
            if( j==match.size() ){
                create=true;
                // Find video link
                for( ; str[i+j]!='"'; ++j ){
                    if( str[i+j]==';' ){
                        create=false;
                        break;
                    }
                    newlink+=str[i+j];
                }
                if( newlink==links.back().videoUrl )
                    create=false;
                if( create ){
                    
                    // Find video title
                    while( str[i+j]!='l' || str[i+j+1]!='e' ){
                        ++j;
                    }
                    int k=0;
                    for( ; k<match2.size(); ++k ){
                        if( str[i+j+k]==match2[k] )
                            continue;
                        else
                            break;
                    }
                    if( k==match2.size() ){
                        while( str[i+j+k]!='"' ){
                            if( str[i+j+k]=='&' ){
                                k+=5;
                                if( str[i+j+k-4]=='q' )
                                    name+='"';
                                continue;
                            }
                            if( str[i+j+k]==';' ){
                                ++k;
                                continue;
                            }
                            name+=str[i+j+k];
                            ++k;
                        }
                    }
                    j+=k;
                    // Find view count
                    while( str[i+j]!='w' || str[i+j+1]!='-' || str[i+j+2]!='c' ){
                        ++j;
                    }
                    k=0;
                    for( ; k<match3.size(); ++k ){
                        if( str[i+j+k]==match3[k] )
                            continue;
                        else
                            break;
                    }
                    if( k==match3.size() ){
                        while( str[i+j+k]<'a' ){
                            if( str[i+j+k]<='9' && str[i+j+k]>='0' )
                                size+=str[i+j+k];
                            ++k;
                        }
                    }
                    j+=k;
                    // Find video time
                    while( str[i+j]!='i' || str[i+j+1]!='m' || str[i+j+2]!='e' ){
                        ++j;
                    }
                    k=0;
                    for( ; k<match4.size(); ++k ){
                        if( str[i+j+k]==match4[k] )
                            continue;
                        else
                            break;
                    }
                    if( k==match4.size() ){
                        while( str[i+j+k]!=':' ){
                            if( str[i+j+k]<='9' && str[i+j+k]>='0' )
                                vTime+=str[i+j+k];
                            ++k;
                        }
                    }
                    j+=k;
                }
            }
            // Make link
            if( create ){
                if( vTime=="" )
                    vTime="0";
                if( size=="" )
                    size+="999999999";
                if( stoi(vTime)<=maxtime )
                    links.push_back({std::stoi(size), newlink, name, vTime});
            }
            i+=j-1;
        }
    }
}

// Check if given url is valid
bool urlCheck(){
    for( int i=0; i<prefix.length(); ++i ){
        if( input[i]==prefix[i] )
            continue;
        else
            return 0;
    }
    return 1;
}

int main(int argc, const char *argv[])
{
    struct timeval time; 
    gettimeofday(&time,NULL);
    srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
    
    printf("Enter video url: ");
	std::cin>>input;
    
    if( !urlCheck() ){
        printf("Url should look like https://www.youtube.com/watch?v=abcdefghijk, not ");
        std::cout<<input<<"\n";
        return 1;
    }
    
    printf("Enter video output level: ");
    scanf("%d", &n);
    
    links.push_back({0, input, "", "0"});
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    for( int i=0; i<=n; ++i ){
        int range=maxrange;
        if( !links.size() )
            return 2;
        int rand;
        
        // Choose new url
        rand=roll(std::min((int)links.size()-1, std::max((int)(links.size()/10), range)));
        std::sort(links.begin(), links.end(), sortOperator);
        int j=0;
        while( visited[links[rand].videoUrl] ){
            rand=roll(std::min((int)links.size()-1, std::max((int)(links.size()/10), range)));
            ++j;
            range=std::max(range, j/100);
            if( j==100000 ){
                return 3;
            }
        }
        urlstr=links[rand].videoUrl;
        visited[urlstr]=true;
        url=urlstr.c_str();

        if( i ){
            printf("%d: ", i);
            std::cout<<links[rand].viewCount<<" "<<links[rand].videoUrl<<" '"<<links[rand].videoName<<"'\n";
        }

        // Get output from url
        if (curl)
        {
            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &stream);
            res = curl_easy_perform(curl);
        }
    	std::string output = stream.str();
    	search(output);
    }
    curl_easy_cleanup(curl);
	return 0;
}
