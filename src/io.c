#include "sigyn.h"

mowgli_linebuf_t * new_conn(const char * host, const char * port,
        mowgli_linebuf_readline_cb_t *cb, void * udata)
{
    struct addrinfo hints, * res;
    mowgli_vio_sockaddr_t addr;
    mowgli_linebuf_t * linebuf;
    int ret;

    linebuf = mowgli_linebuf_create(cb, udata);

    memset(&hints, 0, sizeof hints);
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((ret = getaddrinfo(host, port, &hints, &res)) != 0)
    {
        linebuf->vio->error.op = MOWGLI_VIO_ERR_OP_OTHER;
        linebuf->vio->error.type = MOWGLI_VIO_ERR_ERRCODE;
        linebuf->vio->error.code = ret;
        mowgli_strlcpy(linebuf->vio->error.string, gai_strerror(ret), sizeof(linebuf->vio->error.string));
        mowgli_vio_error(linebuf->vio);
        return NULL;
    }

    if (mowgli_vio_socket(linebuf->vio, res->ai_family, res->ai_socktype, res->ai_protocol) != 0)
        return NULL;

    mowgli_linebuf_attach_to_eventloop(linebuf, me.uplink.ev);

    if (mowgli_vio_connect(linebuf->vio, mowgli_vio_sockaddr_from_struct(&addr, res->ai_addr, res->ai_addrlen)) != 0)
        return NULL;

    return linebuf;
}
