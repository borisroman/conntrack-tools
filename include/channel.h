#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include "mcast.h"

struct channel;
struct nethdr;

enum {
	CHANNEL_MCAST,
	CHANNEL_MAX,
};

struct mcast_channel {
	struct mcast_sock *client;
	struct mcast_sock *server;
};

#define CHANNEL_F_DEFAULT	(1 << 0)
#define CHANNEL_F_BUFFERED	(1 << 1)
#define CHANNEL_F_MAX		(1 << 2)

union channel_type_conf {
	struct mcast_conf mcast;
};

struct channel_conf {
	int				channel_type;
	char				channel_ifname[IFNAMSIZ];
	unsigned int			channel_flags;
	union channel_type_conf		u;
};

struct nlif_handle;

struct channel_ops {
	void *	(*open)(void *conf);
	void	(*close)(void *channel);
	int	(*send)(void *channel, const void *data, int len);
	int	(*recv)(void *channel, char *buf, int len);
	int	(*get_fd)(void *channel);
	void	(*stats)(struct channel *c, int fd);
	void	(*stats_extended)(struct channel *c, int active,
				  struct nlif_handle *h, int fd);
};

struct channel_buffer;

struct channel {
	int			channel_type;
	int			channel_ifindex;
	int			channel_ifmtu;
	unsigned int		channel_flags;
	struct channel_buffer	*buffer;
	struct channel_ops	*ops;
	void			*data;
};

void channel_init(void);
struct channel *channel_open(struct channel_conf *conf);
void channel_close(struct channel *c);

int channel_send(struct channel *c, const struct nethdr *net);
int channel_send_flush(struct channel *c);
int channel_recv(struct channel *c, char *buf, int size);

int channel_get_fd(struct channel *c);
void channel_stats(struct channel *c, int fd);
void channel_stats_extended(struct channel *c, int active,
			    struct nlif_handle *h, int fd);

#define MULTICHANNEL_MAX	4

struct multichannel {
	int		channel_num;
	struct channel *channel[MULTICHANNEL_MAX];
	struct channel *current;
};

struct multichannel *multichannel_open(struct channel_conf *conf, int len);
void multichannel_close(struct multichannel *m);

int multichannel_send(struct multichannel *c, const struct nethdr *net);
int multichannel_send_flush(struct multichannel *c);
int multichannel_recv(struct multichannel *c, char *buf, int size);

void multichannel_stats(struct multichannel *m, int fd);
void multichannel_stats_extended(struct multichannel *m,
				 struct nlif_handle *h, int fd);

int multichannel_get_ifindex(struct multichannel *m, int i);
int multichannel_get_current_ifindex(struct multichannel *m);
void multichannel_set_current_channel(struct multichannel *m, int i);

#endif /* _CHANNEL_H_ */
