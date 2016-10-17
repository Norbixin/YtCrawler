#include <bits/stdc++.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include <sstream>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

const char *url;
std::map<std::string, bool> visited;
std::string urlstr="";
std::string input, match="href=\"/watch", match2="w-count\">", prefix="https://www.youtube.com/watch";
std::vector<std::pair<int, std::string> > links;
boost::random::mt19937 gen;
std::ostringstream stream;
unsigned int n;

size_t write_data(char *ptr, size_t size, size_t nmemb, void *userdata) {
    std::ostringstream *stream = (std::ostringstream*)userdata;
    size_t count = size * nmemb;
    stream->write(ptr, count);
    return count;
}

int roll(int x) {
    boost::random::uniform_int_distribution<> dist(0, x);
    return dist(gen);
}

void search(std::string str){
    links.clear();
    std::string newlink, size;
    for( int i=0; i<str.length(); ++i ){
        if( str[i]=='h' ){
            bool create=false;
            newlink=prefix;
            size="";
            int j=0;
            std::string wypisz;
            for( ; j<match.size(); ++j ){
                if( str[i+j]==match[j] ){
                    wypisz+=str[i+j];
                    continue;
                }
                else
                    break;
            }
            //<span class="stat view-count">8 102 
            if( j==match.size() ){
                create=true;
                // std::cout<<wypisz<<" "<<j<<"\n";
                for( ; str[i+j]!='"'; ++j ){
                    // printf("%c", str[i+j]);
                    if( str[i+j]==';' ){
                        create=false;
                        break;
                    }
                    newlink+=str[i+j];
                }
                if( newlink==links.back().second )
                    create=false;
                if( create ){
                    while( str[i+j]!='w' || str[i+j+1]!='-' || str[i+j+2]!='c' ){
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
                        while( str[i+j+k]<'a' ){
                            if( str[i+j+k]<='9' && str[i+j+k]>='0' )
                                size+=str[i+j+k];
                            ++k;
                        }
                    }
                }
            }
            if( create ){
                if( size=="" )
                    size+="999999999";
                // std::cout<<"a"<<size<<"a"<<"\n";
                links.push_back({std::stoi(size), newlink});
                // std::cout<<newlink<<" "<<size<<"\n";
            }
            i+=j-1;
        }
    }
}

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
    printf("Enter video url: ");
	std::cin>>input;
    if( !urlCheck() ){
        printf("Url should look like https://www.youtube.com/watch?v=abcdefghijk, not ");
        std::cout<<input<<"\n";
        return 1;
    }
    printf("Enter video output level: ");
    scanf("%d", &n);
    links.push_back({99999, input});
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    for( int i=0; i<=n; ++i ){
        int range=7;
        if( !links.size() )
            return 2;
        int rand;
        if( !i )
            rand=0;
        else
            rand=roll(std::min((int)links.size(), std::max((int)(links.size()/10), range)));
        // printf("%d\n", links.size());
        std::sort(links.begin(), links.end());
        int j=0;
        while( visited[links[rand].second] ){
            rand=roll(std::min((int)links.size(), std::max((int)(links.size()/10), range)));
            ++j;
            range=std::max(range, j/100);
            if( j==100000 ){
                return 3;
            }
        }
        urlstr=links[rand].second;
        visited[urlstr]=true;
        url=urlstr.c_str();
        printf("%d: ", i);
        std::cout<<links[rand].first<<" "<<links[rand].second<<"\n";
        if (curl)
        {
            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &stream);
            res = curl_easy_perform(curl);
        }
    	std::string output = stream.str();
    	search(output);
        // std::cout<<output;
    }
    curl_easy_cleanup(curl);
	return 0;
}
