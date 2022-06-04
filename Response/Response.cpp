#include "Response.hpp"


Response::Response(std::vector<ServerConfig> &servers) : MIME_types(get_MIME_types()), status_defin(), servers(servers), headers() {
	status_defin[400] = "400 Bad Request\r\n";
	status_defin[200] = "200 OK\r\n";
	status_defin[201] = "201 Created\r\n";
	status_defin[202] = "202 Accepted\r\n";
	status_defin[204] = "204 No Content\r\n";
	status_defin[401] = "401 Unauthorized\r\n";
	status_defin[403] = "403 Forbidden\r\n";
	status_defin[404] = "404 Not Found\r\n";
	status_defin[405] = "405 Method Not Allowed\r\n";
	status_defin[301] = "301 Moved Permanently\r\n";
	status_defin[302] = "302 Found\r\n";
	status_defin[304] = "304 Not Modified\r\n";

	// std::map<std::string, std::string>::iterator it = MIME_types.begin();
	// while (it != MIME_types.end())
	// {
	// 	std::cout << "[\"" << it->first << "\", \"" << it->second << "\"]" << std::endl;
	// 	it++;
	// }
}


std::string	read_file(std::string root, const char* filename){
	std::ifstream fd;
	if (strcmp(filename, "/") == 0)
		fd.open(root + "/index.php");
	else
		fd.open(root + filename);
	std::cout << root + filename << std::endl;
	if (!fd.is_open())
		std::cout << "Failed" << std::endl;
	std::string ret( (std::istreambuf_iterator<char>(fd) ),
                       (std::istreambuf_iterator<char>()    ) );
	return ret;
}

std::string Response::get_response(request	&req) {
	// std::cout << "-----------------Request--------------" << std::endl;
	// std::map<std::string, std::string> mp = req.getHeaders();
	// std::cout << req.getMethod() << std::endl;
	// std::map<std::string, std::string>::iterator it = mp.begin();
	// while (it != mp.end()) {
	// 	std::cout << "REquest headers = " << it->first << " " << it->second << std::endl;
	// 	it++;
	// }

	std::cout << "request ext = " << req.getUri() << std::endl;
	
	std::string respo;
	int i;
	i = select_server(req);
	if (!req.getExtension().empty()){ // request with file
		std::map<std::string, std::string>::iterator it = MIME_types.find(req.getExtension());
		if (it != MIME_types.end()) { // Regular file
			respo = MIME_response(req, i, it);
		} else { // it's CGI
			respo = CGI_response(req, i);
		}
	} else { // request with Directory
		respo = Dir_response(req, i);
	}
	headers.clear();
	// if (pr.path.find(".php") != std::string::npos) { 
	// 	Cgi_request cgi(req, servers[0]);
	// 	respo = cgi.execute();
	// }
	// else {
	// 	respo = "HTTP/1.1 200 OK\r\n\r\n";
	// 	std::string tmp (req.getHeaders().find("Accept")->second);
	// 	respo += read_file(getenv("PWD"), pr.path.c_str());
	// }
	return respo;
}

int	Response::select_server(request &req){
	std::stringstream ss;
	std::string Host;
	std::map<std::string, std::string> head = req.getHeaders();
	std::map<std::string, std::string>::iterator it = head.find("Host");
	if (it != head.end())
		Host = req.getHeaders().find("Host")->second;
	else
		return 0;
	size_t i = Host.find(":");
	std::vector<int> list_servers_p; // list of Servers with the same port by index
	std::vector<int> list_servers_n; // list of Servers with the same name by index

	ss << Host.begin().base() + i + 1;
	int port;
	ss >> port;
	std::string server_name;
	// std::cout << "HOST = " << Host << " i = " << i << std::endl;
	try {
		server_name.assign(Host.begin(), Host.begin() + i);
	} catch (std::exception &e) {
		std::cout << "Exception Here = " << e.what() << std::endl;
	}

	for (size_t j = 0; j < servers.size(); j++)  // Looking in servers by Port if there is more than one
	{
		if (servers[j].get_port() == port)
			list_servers_p.push_back(j);
	}
	if (list_servers_p.size() > 1) // there is more than one server with the same port
	{
		for (size_t j = 0; j < list_servers_p.size(); j++) {
			if (servers[list_servers_p[j]].get_name() == server_name)
				list_servers_n.push_back(list_servers_p[j]);
		}
		return list_servers_n[0];
	}
	return list_servers_p[0];
}

std::string	Response::MIME_response(request &req, int i, std::map<std::string, std::string>::iterator &it) {
	std::string respo;
	std::vector<std::string> headers;
	std::string filename(servers[i].get_root() + req.getUri()), length;
	std::stringstream ss;
	std::ifstream file(filename);

	std::cout << "MIME Filename = " << filename << std::endl;
	if (file.is_open())
	{
		std::string content((std::istreambuf_iterator<char>(file) ), (std::istreambuf_iterator<char>() ));
		ss << content.length();
		ss >> length;
		headers.push_back("Content-Type: " + it->second);
		headers.push_back("Content-Length: " + length);
		return (make_response(200, headers, content));
	}
	return std::string();
}

std::string	Response::CGI_response(request &req, int i){
	std::string respo;
	std::cout << "ROOT = " << servers[i].get_root() << std::endl;
	std::string filename(servers[i].get_root() + req.getUri());

	std::cout << "CGI Filename = " << filename << std::endl;
	// std::cout << " BODY = " << req.getFilename() << std::endl;
	// std::ifstream file(req.getFilename());
	// if (file.is_open())
	// {
	// 	std::string line;
	// 	while(std::getline(file, line))
	// 		std::cout << line << std::endl;
	// }
	Cgi_request cgi(req, servers[i]);
	respo = cgi.execute();
	return respo;
}

std::string	Response::Dir_response(request &req, int i){
	std::string respo, ser_root, loca_root;
	std::string filename(servers[i].get_root() + req.getUri());
	int loc_i;

	std::cout << "Dir Filename = " << req.getUri() << std::endl;
	loc_i = find_location(req.getUri(), i);
	ser_root = servers[i].get_root();
	if (!ser_root.empty() && ser_root[ser_root.size() - 1] == '/')
		ser_root.erase(ser_root.end() - 1);
	/* LOCATION FOUNDED */
	if (loc_i != -1) {
		loca_root = servers[i].getLocation()[loc_i].get_root();
		if (!loca_root.empty() && loca_root[loca_root.size() - 1] != '/')
			loca_root += "/";
		if (!is_allowed(req.getMethod(), servers[i].getLocation()[loc_i]))
			respo = make_error_response(405);
		else {
			if (req.getMethod() == "GET"){
				if (loca_root.empty()) // Location without root
					respo = make_redirection(304, ser_root + req.getUri());
				else { // Location with root
					respo = make_redirection(301, loca_root);
				}
			}
			else if (req.getMethod() == "POST"){}
			else {}
		}
	}
	else { // There is no Location with this Name

	}
	return respo;
}

std::string	Response::make_response(int status, std::vector<std::string> &headers, std::string &body) {
	std::string response("HTTP/1.1 ");

	response += status_code(status);
	for(int i = 0; i < headers.size(); i++) {
		response += headers[i] + "\r\n";
	}
	response += "\r\n" + body;
	return response;
}

std::string	Response::status_code(int status){
	std::stringstream ss;
	std::string tmp;
	std::map<int, std::string>::iterator it = status_defin.find(status);
	if (it != status_defin.end())
		return it->second;
	return std::string ("200 OK\r\n");
}

int		Response::find_location(std::string name, int i) {
	std::vector<LocationConfig> locs = servers[i].getLocation();
	size_t j = 0;

	for (; j < locs.size(); j++) {
		if (locs[j].get_name() == name)
			return j;
	}
	return -1;
}

bool			Response::is_allowed(std::string method, LocationConfig location)
{
	std::vector<std::string> methods = location.getMethods();
	if (methods.empty())
		return true;
	for (size_t i = 0; i < methods.size(); i++){
		if (method == methods[i])
			return true;
	}
	return false;
}

std::string		Response::make_error_response(int status) {
	std::string respo ("HTTP/1.1 ");
	std::stringstream ss(status);
	std::string	code, length, filename("errors/"), body;
	std::ifstream	file;
	ss >> code;

	filename += code + ".html";
	file.open(filename);
	body.append( (std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()) );
	ss.clear();
	ss << body.length();
	ss >> length;
	respo += status_defin[status] + "\r\n";
	respo += "Content-Type: " + (MIME_types[".html"] + "\r\n");
	respo += "Content-Length: " + (length + "\r\n\r\n");
	respo += body;
	return respo;
}

std::string		Response::make_redirection(int status, std::string to){
	std::string respo ("HTTP/1.1 ");

	respo += status_defin.find(status)->second;
	respo += "Location: " + (to + "\r\n");
	respo += "Content-Length: 0\r\n\r\n";

	return respo;
}