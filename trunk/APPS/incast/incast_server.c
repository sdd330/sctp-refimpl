#include <incast_fmt.h>


void
process_a_child(int sd, struct sockaddr_in *sin, int use_sctp)
{
	int cnt, sz;
	int *p;
	char buffer[MAX_SINGLE_MSG];
	struct timespec tvs, tve;
	struct incast_msg_req inrec;
	int no_clock_s, no_clock_e, i;
	socklen_t optlen;
	int optval;
	ssize_t readin, sendout;
	if(clock_gettime(CLOCK_MONOTONIC_PRECISE, &tvs))
		no_clock_s = 1;
	else 
		no_clock_s = 0;
	optval = 1;
	optlen = sizeof(optval);
	if (use_sctp) {
		if(setsockopt(sd, IPPROTO_SCTP, SCTP_NODELAY, &optval, optlen)) {
			printf("Warning - can't turn on nodelay for sctp err:%d\n",
			       errno);
		}
	} else {
		if(setsockopt(sd, IPPROTO_TCP, TCP_NODELAY, &optval, optlen)) {
			printf("Warning - can't turn on nodelay for tcp err:%d\n",
			       errno);
		}
	}
	/* Find out how much we must send */
	errno = 0;
	readin = recv(sd, &inrec, sizeof(inrec), 0);
	if(readin != sizeof(inrec)) {
		printf("Did not get %ld bytes got:%ld err:%d\n",
		       (long int)sizeof(inrec), (long int)readin, errno);
		goto out;
	}
	cnt = htonl(inrec.number_of_packets);
	sz = htonl(inrec.size);
	/* How big must the socket buffer be? */
	optlen = sizeof(optval);
	optval = (cnt * sz) + 1;
	if(setsockopt(sd, SOL_SOCKET, SO_SNDBUF, &optval, optlen) != 0){
		printf("Warning - could not grow sock buf to %d will block err:%d\n",
		       optval,
		       errno);
	}
	/* protect against bad buffer sizes */
	if (sz > MAX_SINGLE_MSG) {
		cnt = (optval / MAX_SINGLE_MSG) + 1;
		sz = MAX_SINGLE_MSG;
	}
	memset(buffer, 0, sizeof(buffer));
	p = (int *)&buffer;
	*p = 1;
	for(i=0; i<cnt; i++) {
		sendout = send(sd, buffer, sz, 0);
		if (sendout < sz) {
			printf("Error sending %d\n", errno);	
			goto out;
		}
		*p = *p + 1;
	}
	if(clock_gettime(CLOCK_MONOTONIC_PRECISE, &tve))
		no_clock_e = 1;
	else 
		no_clock_e = 0;
	if ((no_clock_e == 0) && (no_clock_s == 0)) {
		timespecsub(&tve, &tvs);
		printf("%d rec of %d in %ld.%9.9ld\n",
		       cnt, sz, (long int)tve.tv_sec, tve.tv_nsec);
	}
    out:
	close(sd);
}

int
main(int argc, char **argv)
{
	struct sockaddr_in lsin, bsin;
	int sd, nsd, i, temp;
	uint16_t port = htons(DEFAULT_SVR_PORT);
	int use_sctp=0;
	int backlog=4;
	socklen_t slen;
	char *bindto = NULL;
	while ((i = getopt(argc, argv, "B:b:tsp:?")) != EOF) {
		switch (i) {
		case 'B':
			backlog = strtol(optarg, NULL, 0);
			if (backlog < 1) {
				printf("Sorry backlog must be 1 or more - using default\n");
				backlog = 4;
			}
			break;
		case 'b':
			bindto = optarg;
			break;
		case 'p':
			temp = strtol(optarg, NULL, 0);
			if ((temp < 1) || (temp > 65535)) {
				printf("Error port %d invalid - using default\n",
					temp);
			} else {
				port = htons((uint16_t)temp);
			}
			break;
		case 's':
			use_sctp = 1;
			break;
		case 't':
			use_sctp = 0;
			break;
		default:
		case '?':
		use:
			printf("Use %s -b bind_address[-p port -t -s -B backlog]\n", 
			       argv[0]);
			exit(-1);
			break;
		};
	}
	/* Did they forget the bind address? */
	if (bindto == NULL) {
		goto use;
	}
	/* setup bind address */
	memset(&lsin, 0, sizeof(lsin));
	if(translate_ip_address(bindto, &lsin)) {
		printf("bind to address is invalid - can't translate '%s'\n",
		       bindto);
		exit(-1);
	}
	lsin.sin_port = port;

	/* Which protocol? */
	if (use_sctp) {
		sd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
	} else {
		sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	}

	/* Now lets bind it if we can */
	slen = sizeof(lsin);
	if (bind(sd, (struct sockaddr *)&lsin, slen)) {
		printf("Bind fails errno:%d\n", errno);
		close(sd);
		exit(-1);
	}

	/* Validate that we got our address */
	slen = sizeof(bsin);
	if (getsockname(sd, (struct sockaddr *)&bsin, &slen)) {
		printf("Get socket name fails errno:%d\n", errno);
		close(sd);
		exit(-1);
	}
	if (bsin.sin_port != lsin.sin_port) {
		printf("Bound port is incorrect bound:%d wanted:%d\n",
		       ntohs(bsin.sin_port), ntohs(lsin.sin_port));
		close(sd);
		exit(-1);
	}

	/* now lets listen */
	if (listen(sd, backlog)) {
		printf("Listen fails err:%d\n", errno);
		close(sd);
		exit(-1);
	}
	/* Loop forever processing connections */
	while(1) {
		slen = sizeof(bsin);
		nsd = accept(sd, (struct sockaddr *)&bsin, &slen);
		process_a_child(nsd, &bsin, use_sctp);
	}
	return (0);
}
