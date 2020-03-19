#include "getx.h"
#undef max

typedef struct bitrange
{
	int start;
	int end;
}bitrange;

int main()
{
	printf("--------------getx, threaded HTTP file downloader--------------\n");
	printf("-------------------getx 2.0 by Skyler Nelson-------------------\n");

	char input_url[1024];
	while (true) {
		if (!std::cin.getline(input_url, sizeof(input_url))) {
			printf("error, enter a URL/file\n");

			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		} else {
			size_t fsize = HTTPQueryFileSize(std::string(input_url));

			if (fsize <= 0) {
				printf("Target resource either doesn't exist or you don't have proper authentication\n");

				continue;
			}

			const int chunksz = fsize / 10;

			printf("File size: %i\nChunking size: %i\n", fsize, chunksz);

			for (int i = 1; i < fsize; i += chunksz) 
			{
				if (i == 1) {
					printf("start: %i\nend: %i\n\n", i, (i+1) + chunksz);
				} else {
					if (((i + 1) + chunksz) > fsize) {
						printf("start: %i\nend: %i\n\n", i + 2, fsize);
					} else {
						printf("start: %i\nend: %i\n\n", i + 2, (i + 1) + chunksz);
					}
				}
			}
		}
	}

	return NULL;
}

size_t HTTPQueryFileSize(std::string url)
{
	auto hostbuff = strExp(url, '/');
	auto host	  = hostbuff[0];
	auto argv	  = url.substr(host.length(), url.length());

	WSADATA winsock_data;
	WSAStartup(MAKEWORD(2, 2), &winsock_data);

	hostent* hp   = gethostbyname(host.c_str());
	if (hp == nullptr) return NULL;

	in_addr addr;
	addr.S_un.S_addr = *(u_long*)hp->h_addr_list[0];

	WSACleanup();

	csockdata sockinfo;
	sockinfo.address	  = inet_ntoa(addr);
	sockinfo.port		  = "80";
	sockinfo.dataprotocol = CSOCKET::CSOCKET_TCP;
	sockinfo.ipprotocol	  = CSOCKET::CSOCKET_IPV4;
	sockinfo.socktype     = CSOCKET::CSOCKET_SIMPLE;

	CSOCKET* sv = new CSOCKET(&sockinfo);
	if (!sv->IsValid()) return NULL;

	sv->Connect();

	auto rq = std::string(
	  "HEAD " + argv + " HTTP/1.1\r\n"
	+ "User-Agent: getx http downloader\r\n"
	+ "Accept: */*\r\n"
	+ "Connection: keep-alive\r\n\r\n"
	);
	sv->Send(rq.data(), rq.length());

	char* buff = new char[8096];
	while (int got = sv->Recv(buff, 8096)) {
		if (got == CSOCKET::CSOCKET_FATAL_ERROR)
			break;

		printf("\n%s\n", buff);

		auto http_header = strExp(buff, '\r\n');
		if (http_header[0].find("404") == 1) return 0;

		for (int i = 0; i < http_header.size(); i++) {
			if (http_header[i].find("Content-Length:") == 1) {
				auto sizedets = strExp(http_header[i], ':');

				return std::stoi(sizedets[1].substr(1, sizedets[1].size()).data());
			}
		}
	}

	return 0;
}

std::vector<std::string> strExp(std::string const& s, char delim)
{
	std::string buff{ "" };
	std::vector<std::string> v;

	for (auto n : s) {
		if (n != delim) buff += n; else
			if (n == delim && buff != "") { v.push_back(buff); buff = ""; }
	}

	if (buff != "") v.push_back(buff);

	return v;
}
