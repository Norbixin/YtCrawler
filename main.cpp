#include <bits/stdc++.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include <sstream>

#define MAX_RANGE    10 // how many url's are considered for the next video
#define MAX_TIME     20 // maximum length of video in minutes
#define MIN_TIME      0 // maximum length of video in minutes
#define MAX_NAME_LEN 35 // maximum video name length in characters

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
unsigned outputLevel;

bool sortOperator(slink a, slink b) {
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
void search(std::string str) {
	links.clear();
	std::string newlink, size, name, vTime;
	for( unsigned i=0; i<str.length(); ++i ) {
		if( str[i]=='h' ) {
			bool create=false;
			newlink=prefix;
			size=name=vTime="";
			unsigned j=0;
			for( ; j<match.size(); ++j ) {
				if( str[i+j]==match[j] ) {
					continue;
				}
				else
					break;
			}
			// Make link
			if( j==match.size() ) {
				create=true;
				// Find video link
				for( ; str[i+j]!='"'; ++j ) {
					if( str[i+j]==';' ) {
						create=false;
						break;
					}
					newlink+=str[i+j];
				}
				if( newlink==links.back().videoUrl )
					create=false;
				if( create ) {
					
					// Find video title
					while( str[i+j]!='l' || str[i+j+1]!='e' ) {
						++j;
					}
					unsigned k=0;
					for( ; k<match2.size(); ++k ) {
						if( str[i+j+k]==match2[k] )
							continue;
						else
							break;
					}
					if( k==match2.size() ) {
						while( str[i+j+k]!='"' ) {
							if( str[i+j+k]=='&' ) {
								k+=5;
								if( str[i+j+k-4]=='q' )
									name+='"';
								continue;
							}
							if( str[i+j+k]==';' ) {
								++k;
								continue;
							}
							name+=str[i+j+k];
							++k;
						}
					}
					j+=k;
					// Find view count
					while( str[i+j]!='w' || str[i+j+1]!='-' || str[i+j+2]!='c' ) {
						++j;
					}
					k=0;
					for( ; k<match3.size(); ++k ) {
						if( str[i+j+k]==match3[k] )
							continue;
						else
							break;
					}
					if( k==match3.size() ) {
						while( str[i+j+k]<'a' ) {
							if( str[i+j+k]<='9' && str[i+j+k]>='0' )
								size+=str[i+j+k];
							++k;
						}
					}
					j+=k;
					// Find video time
					while( str[i+j]!='i' || str[i+j+1]!='m' || str[i+j+2]!='e' ) {
						++j;
					}
					k=0;
					for( ; k<match4.size(); ++k ) {
						if( str[i+j+k]==match4[k] )
							continue;
						else
							break;
					}
					if( k==match4.size() ) {
						while( str[i+j+k]!=':' ) {
							if( str[i+j+k]<='9' && str[i+j+k]>='0' )
								vTime+=str[i+j+k];
							++k;
						}
					}
					j+=k;
				}
			}
			// Make link
			if( create && !visited[newlink] ) {
				if( vTime=="" )
					vTime="0";
				if( size=="" )
					size+="999999999";
				if( stoi(vTime)<=MAX_TIME && stoi(vTime)>=MIN_TIME )
					links.push_back({std::stoi(size), newlink, name, vTime});
			}
			i+=j-1;
		}
	}
}

// Check if given url is valid
bool urlCheck() {
	for( unsigned i=0; i<prefix.length(); ++i ) {
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
	
	std::cout<<"Paste video url here: ";
	std::cin>>input;
	
	if( !urlCheck() ) {
		std::cout << "URL's should look like this:" << std::endl;
		std::cout << "https://www.youtube.com/watch?v=abcdefghijk" << std::endl;
		return 1;
	}
	
	std::cout << "How many videos to crawl through? ";
	std::cin >> outputLevel;
	
	// Used for displaying info
	unsigned maxIndexLen = std::to_string(outputLevel).length();
	unsigned maxViewLen = 0;
	std::string indStr, viewStr, nameStr, urlStr;
	std::string empty;
	for(unsigned ind = 0; ind < MAX_NAME_LEN; ++ind)
		empty += " ";

	links.push_back({0, input, "", "0"});
	CURL *curl = curl_easy_init();;
	CURLcode res; // this throws a warning
	// main.cpp: In function ‘int main(int, const char**)’:
	// main.cpp:197:11: warning: variable ‘res’ set but not used [-Wunused-but-set-variable]
	// CURLcode res;

	for( unsigned i=0; i<=outputLevel; ++i ) {
		int range=MAX_RANGE;
		if( !links.size() )
			return 2;
		int rand;
		
		// Choose new url
		rand=roll(std::min((int)links.size()-1, std::max((int)(links.size()/10), range)));
		std::sort(links.begin(), links.end(), sortOperator);
		urlstr=links[rand].videoUrl;
		visited[urlstr]=true;
		url=urlstr.c_str();

		if( i ) {
			// Video info
			// Index
			indStr = std::to_string(i);
			for(unsigned ind = indStr.length(); ind < maxIndexLen; ind++)
				indStr = " " + indStr;
			// Name
			// Note that some characters are multibyte (length() of "♥" is 3, not 1)
			// TODO fix multibyte chars
			nameStr = links[rand].videoName;
			if(nameStr.length() >= MAX_NAME_LEN) 
				nameStr = nameStr.substr(0, MAX_NAME_LEN - 1) + "…";
			else {
				nameStr += empty;
				nameStr = nameStr.substr(0, MAX_NAME_LEN);
			}
			// URL
			// I'm not entirely sure all url are the same length
			urlStr = links[rand].videoUrl; 
			// View count
			viewStr = std::to_string(links[rand].viewCount);
			if(maxViewLen < viewStr.length())
				maxViewLen = viewStr.length();
			for(unsigned ind = viewStr.length(); ind < maxViewLen; ind++)
				viewStr = " " + viewStr;
			viewStr += " Views";
			// Display
			std::cout << indStr << " | " << nameStr << " | " << urlStr << " | " << viewStr << std::endl;
		}

		// Get output from url
		if (curl) {
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
