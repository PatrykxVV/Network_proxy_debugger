#pragma once

//Uruchomienie prostego TCP proxy.
//listen_port - port gdzie proxy nasluchuje clients
//backend_ip - adres ip backendowego serwera
//backend_prot - port backendowego serwera

//client -> proxy -> backend, backend -> proxy -> client

void run_proxy(int listen_port,
                const char* backend_ip,
                int backend_port);