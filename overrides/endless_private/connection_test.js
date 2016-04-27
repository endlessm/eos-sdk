// Copyright (C) 2016 Endless Mobile, Inc.

const Gio = imports.gi.Gio;

// Performs a connection test by first looking for an available connection, and
// performing a ping test with address parameters specified by hostname, scheme,
// and port. The test is performed asynchronously.
// successCallback, failureCallback, and errorCallback are optional parameters
// and are called in their relevent outcome cases. If the ping test completed,
// the userData will be passed to the callbacks. Additionally, the errorCallback
// will recieve the error as the first argument.
// If an errorCallback is not provided, errors will be re-thrown (with the 
// exception of a Gio.ResolverError, which is a connection failure).
function doConnectionTestAsync(hostname, scheme, port, connectionSuccessCallback,
                                                       connectionFailureCallback,
                                                       errorCallback) {
    let network_monitor = Gio.NetworkMonitor.get_default();
    let network_address = new Gio.NetworkAddress({
        'hostname': hostname,
        'scheme': scheme,
        'port': port
    });

    if (network_monitor.get_network_available()) {
        network_monitor.can_reach_async(network_address, null,
                                        function(userData, asyncResult) {
            try {
                let ping_success = network_monitor.
                                   can_reach_finish(asyncResult, null);
                if (ping_success && connectionSuccessCallback) {
                    connectionSuccessCallback(userData);
                } else if (!ping_success && connectionFailureCallback) {
                    connectionFailureCallback(userData);
                }
            } catch (err) {
                if (err instanceof Gio.ResolverError) {
                    connectionFailureCallback(userData);
                } else if (errorCallback) {
                    errorCallback(err, userData);
                } else {
                    throw err;
                }
            }
        });
    } else if (connectionFailureCallback) {
        connectionFailureCallback();
    }
}
