#include "sigyn.h"

mowgli_vio_t * new_connection(const char * host, const char * port)
{
    int ret;
    mowgli_vio_t * vio;
    mowgli_vio_sockaddr_t * addr;
    struct addrinfo hints, * res;
    mowgli_vio_evopts_t evopts;

    vio = mowgli_vio_create(NULL);
    evopts = {
        .read_cb = reader,
        .write_cb = writer
    };

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((ret = getaddrinfo(host, port, &hints, &res)) != 0)
    {
        vio->error.op = MOWGLI_VIO_ERR_OP_OTHER;
        vio->error.type = MOWGLI_VIO_ERR_ERRCODE;
        vio->error.code = ret;

        mowgli_strlcpy(vio->error.string, gai_strerror(ret), 
                sizeof(vio->error.string));
        mowgli_vio_error(vio);

        return NULL;
    }

    return_if_fail(!(mowgli_vio_socket(vio, res->ai_family, 
                    res->ai_socktype, res->ai_protocol)));

    mowgli_vio_eventloop_attach(vio, me->ev, evopts);

    return_if_fail(!(mowgli_vio_connect(vio, 
                    mowgli_vio_sockaddr_from_struct(&addr, res->ai_addr,
                        res->addrlen))));

    return vio;

}
