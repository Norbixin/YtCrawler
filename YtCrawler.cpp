#include <bits/stdc++.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include <sstream>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
const char *url;
std::string input, match="href=\"/watch", prefix="https://www.youtube.com/watch";
std::vector<std::string> links;
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
    std::string newlink;
    for( int i=0; i<str.length(); ++i ){
        if( str[i]=='h' ){
            bool create=false;
            newlink=prefix;
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
            if( j==match.size() ){
                create=true;
                // std::cout<<wypisz<<" "<<j<<"\n";
                for( ; str[i+j]!='"'; ++j ){
                    // printf("%c", str[i+j]);
                    newlink+=str[i+j];
                }
            }
            if( create ){
                links.push_back(newlink);
                // std::cout<<newlink<<"\n";
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
    links.push_back(input);
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    for( int i=0; i<=n; ++i ){
        if( !links.size() )
            return 2;
        int rand=roll(links.size()-1);
        url=links[rand].c_str();
        printf("%d: ", i);
        std::cout<<links[rand]<<"\n";
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
    // std::cout<<output;
	return 0;
}
