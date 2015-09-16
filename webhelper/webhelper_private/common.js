// Copyright 2015 Endless Mobile, Inc.

// The following constant and two modules must be in the global namespace before
// importing this file: LIBEXEC_SUBDIR, WebKit2, WebHelper2Private

/* exported WebHelper */

const Format = imports.format;
const Gio = imports.gi.Gio;
const GLib = imports.gi.GLib;
const GObject = imports.gi.GObject;
const Lang = imports.lang;

String.prototype.format = Format.format;

const Config = imports.webhelper_private.config;

const WH2_URI_SCHEME = 'webhelper';
const WH2_LOCAL_FILE_SCHEME = 'local';
const DBUS_WEBVIEW_EXPORT_PATH = '/com/endlessm/webview/';
const WH2_DBUS_EXTENSION_INTERFACE = '\
    <node> \
        <interface name="com.endlessm.WebHelper2.Translation"> \
            <method name="Translate"/> \
        </interface> \
    </node>';
const WH2_DBUS_MAIN_PROGRAM_INTERFACE = '\
    <node> \
        <interface name="com.endlessm.WebHelper2.Gettext"> \
            <method name="Gettext"> \
                <arg name="message" type="s" direction="in"/> \
                <arg name="translation" type="s" direction="out"/> \
            </method> \
            <method name="NGettext"> \
                <arg name="message_singular" type="s" direction="in"/> \
                <arg name="message_plural" type="s" direction="in"/> \
                <arg name="number" type="t" direction="in"/> \
                <arg name="translation" type="s" direction="out"/> \
            </method> \
        </interface> \
    </node>';


/**
 * Namespace: WebHelper2
 * Convenience library for running web applications
 *
 * WebHelper is a convenience library for displaying web applications inside a
 * GTK container running WebKitGTK.
 * WebHelper2 is the WebKit2 version.
 *
 * Although WebKit provides an easy way of communicating from GTK code to
 * the in-browser Javascript, through the execute_script() method, it is not so
 * easy to communicate the other way around.
 *
 * WebHelper solves that problem by detecting when the web page navigates to a
 * custom action URI.
 * The custom URI corresponds to a function that you define using
 * <WebHelper.define_web_action()>, and you can pass parameters to the
 * function.
 *
 * Another often-encountered problem is localizing text through the same API as
 * your main GTK program.
 * WebHelper solves this problem by allowing you to mark strings in your HTML
 * page and translating them through a function of your choice when you run
 * <WebHelper.translate_html()>.
 * It also exposes a *gettext()* function in the client-side Javascript.
 *
 * For cases where you need to load local files for your web applications,
 * WebHelper also provides the local:// URI scheme.
 * For this to work, you must also load your main page via the local:// URI
 * scheme.
 */

/**
 * Class: WebHelper
 * Helper object for a WebKit2 web application
 *
 * Constructor parameters:
 *   props - a dictionary of construction properties and values (default {})
 *
 * The application class for your web application should create <WebHelper> after
 * registering itself in the session bus (i.e. not inside *vfunc_dbus_register()*),
 * with appropriate <well-known-name> and <connection> parameters.
 * A good place to do this would be in your *vfunc_startup()* implementation, where
 * you can also do further setup on it, such as defining web actions.
 *
 * There is no need to set up specially any web views that you create, unlike
 * WebKit1 where you must set up <Application.web_actions_handler()>.
 *
 * Example:
 * > const TestApplication = new Lang.Class({
 * >    Name: 'TestApplication',
 * >    Extends: Gtk.Application,
 * >
 * >    vfunc_dbus_register: function (connection, object_path) {
 * >        this._webhelper = new WebHelper2.WebHelper({
 * >            well_known_name: this.application_id,
 * >            connection: connection,
 * >        });
 * >        return this.parent(connection, object_path);
 * >    },
 * >
 * >    vfunc_startup: function () {
 * >        this.parent();
 * >
 * >        this._webhelper.set_gettext(Gettext.dgettext.bind(null,
 * >            GETTEXT_DOMAIN));
 * >
 * >        let window = new Gtk.Window();
 * >        let webview = new WebKit2.WebView();
 * >        webview.connect('load-changed', (webview, event) => {
 * >            if (event === WebKit2.LoadEvent.FINISHED)
 * >                this._webhelper.translate_html(webview, null, (obj, res) => {
 * >                    this._webhelper.translate_html_finish(res);
 * >                    window.show_all();
 * >                });
 * >        });
 * >        window.add(webview);
 * >        webview.load_uri('file:///path/to/my/page.html');
 * >    },
 * >
 * >    vfunc_dbus_unregister: function (connection, object_path) {
 * >        this.parent(connection, object_path);
 * >        this._webhelper.unregister();
 * >    },
 * >});
 * >
 * >let app = new TestApplication({
 * >    application_id: 'com.example.SmokeGrinder',
 * >});
 * >app.run(ARGV);
 */
const WebHelper = new Lang.Class({
    Name: 'WebHelper',
    GTypeName: 'Wh2WebHelper',
    Extends: GObject.Object,
    Properties: {
        /**
         * Property: well-known-name
         * Well-known bus name owned by the calling program
         *
         * Type:
         *   string
         *
         * This property is required at construction time.
         * It must conform to <the rules for well-known bus names at
         * http://dbus.freedesktop.org/doc/dbus-specification.html#message-protocol-names>.
         *
         * This must be a well-known bus name that your program owns.
         * The easiest way to ensure that is to use your application's ID, since
         * every application class registers its ID as a bus name.
         */
        'well-known-name': GObject.ParamSpec.string('well-known-name',
            'Well-known name',
            'Well-known bus name owned by the calling program',
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT_ONLY,
            ''),
        /**
         * Property: connection
         * DBus connection owned by the calling program
         *
         * Type:
         *   *Gio.DBusConnection*
         *
         * This property is required at construction time.
         *
         * This must be a DBus connection object that your program owns.
         * The easiest way to ensure that is to use the connection object passed
         * in to your application's *vfunc_dbus_register()* function.
         */
        'connection': GObject.ParamSpec.object('connection', 'Connection',
            'DBus connection owned by the calling program',
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT_ONLY,
            Gio.DBusConnection.$gtype),
    },

    _init: function (props={}) {
        this._web_actions = {};
        this._gettext = null;
        this._ngettext = null;
        this._ProxyConstructor =
            Gio.DBusProxy.makeProxyWrapper(WH2_DBUS_EXTENSION_INTERFACE);
        this.parent(props);

        if (this.well_known_name === '')
            throw new Error('The "well-known-name" parameter is required.');
        this._extension_name = this.well_known_name + '.webhelper';

        // Set up Webkit to load our web extension
        let context = window.WebKit2.WebContext.get_default();
        context.connect('initialize-web-extensions', () => {
            let libexec = Gio.File.new_for_path(Config.LIBEXECDIR);
            let path = libexec.get_child(window.LIBEXEC_SUBDIR).get_path();

            let localpath = GLib.getenv('WEBHELPER_UNINSTALLED_EXTENSION_DIR');
            if (localpath)
                path = localpath;

            context.set_web_extensions_directory(path);
            context.set_web_extensions_initialization_user_data(new GLib.Variant('s',
                this.well_known_name));
        });

        // Export our own DBus interface
        this._dbus_impl =
            Gio.DBusExportedObject.wrapJSObject(WH2_DBUS_MAIN_PROGRAM_INTERFACE,
                this);
        this._dbus_impl.export(this.connection, '/com/endlessm/gettext');

        // Set up handling for custom URIs
        window.WebHelper2Private.register_uri_scheme(WH2_URI_SCHEME,
            this._on_endless_uri_request.bind(this));
        window.WebHelper2Private.register_uri_scheme(WH2_LOCAL_FILE_SCHEME,
            this._on_local_file_request.bind(this));
    },

    _on_endless_uri_request: function (request) {
        let uri = request.get_uri();

        // get the name and parameters for the desired function
        let f_call = uri.substr((WH2_URI_SCHEME + '://').length).split('?');
        let function_name = decodeURI(f_call[0]);

        if (!this._web_actions.hasOwnProperty(function_name))
            throw new Error(('Undefined WebHelper action "%s". Did you define it with ' +
                'WebHelper.Application.define_web_action()?').format(function_name));

        let parameters = {};
        if (f_call[1]) {
            // there are parameters
            let params = f_call[1].split('&');
            params.forEach(function (entry) {
                let param = entry.split('=');

                if (param.length == 2) {
                    param[0] = decodeURIComponent(param[0]);
                    param[1] = decodeURIComponent(param[1]);
                    // and now we add it...
                    parameters[param[0]] = param[1];
                }
            });
        }

        (this._web_actions[function_name].bind(this))(parameters);

        // Don't call request.finish(), because we don't want to finish the
        // action, which would involve loading a new page. The request dies
        // if we return from this function without calling ref() or finish()
        // on it.
    },

    _on_local_file_request: function (request) {
        let path = request.get_path();
        let file = Gio.File.new_for_path(path);
        let [content_type, certain] = Gio.content_type_guess(path, null);
        try {
            let stream = file.read(null);
            request.finish(stream, -1, content_type);
        } catch (error) {
            request.finish_error(error);
        }
    },

    // DBus implementations

    Gettext: function (string) {
        return this._gettext(string);
    },

    NGettext: function (singular, plural, number) {
        return this._ngettext(singular, plural, number);
    },

    // Public API

    /**
     * Method: set_gettext
     * Define function which translates text
     *
     * Parameters:
     *   gettext_func - a function, or null
     *
     * When you plan to translate text in your web application, set this
     * property to the translation function.
     * The function must take one parameter, a string, and also return a
     * string.
     * The canonical example is gettext().
     *
     * This function will be called with each string to translate when you call
     * <translate_html()>.
     * The function is also made available directly to the browser-side
     * Javascript as *gettext()*, a property of the global object.
     *
     * Pass null for _gettext_func_ to unset the translation function.
     *
     * If this function has not been called, or has most recently been called
     * with null, then it is illegal to call <translate_html()>.
     *
     * Example:
     * > const Gettext = imports.gettext;
     * > const GETTEXT_DOMAIN = 'smoke-grinder';
     * >
     * > webhelper.set_gettext(Gettext.dgettext.bind(null, GETTEXT_DOMAIN));
     */
    set_gettext: function (gettext_func) {
        if (gettext_func !== null && typeof gettext_func !== 'function')
            throw new Error('The translation function must be a function, or ' +
                'null.');
        this._gettext = gettext_func;
    },

    /**
     * Method: get_gettext
     * Retrieve the currently set translation function
     *
     * Returns:
     *   the translation function previously set with <set_gettext()>, or null
     *   if none is currently set.
     */
    get_gettext: function () {
        return this._gettext;
    },

    /**
     * Method: set_ngettext
     * Define function which translates singular/plural text
     *
     * Parameters:
     *   ngettext_func - a function, or null
     *
     * When you plan to translate text in your web application, set this
     * property to the translation function.
     * The function must take three parameters: a string singular message, a
     * string plural message, and a number for which to generate the message.
     * The function must return a string.
     * The canonical example is ngettext().
     *
     * This function is made available directly to the browser-side Javascript
     * as *ngettext()*, a property of the global object.
     *
     * Pass null for _ngettext_func_ to unset the translation function.
     *
     * If this function has not been called, or has most recently been called
     * with null, then it is illegal to call *ngettext()* in the client-side
     * Javascript.
     *
     * Example:
     * > const WebHelper2 = imports.webhelper2;
     * > const Gettext = imports.gettext;
     * > const GETTEXT_DOMAIN = 'smoke-grinder';
     * >
     * > let webhelper = new WebHelper2.WebHelper('com.example.SmokeGrinder');
     * > webhelper.set_gettext(Gettext.dngettext.bind(null, GETTEXT_DOMAIN));
     */
    set_ngettext: function (ngettext_func) {
        if (ngettext_func !== null && typeof ngettext_func !== 'function')
            throw new Error('The translation function must be a function, or ' +
                'null.');
        this._ngettext = ngettext_func;
    },

    /**
     * Method: get_ngettext
     * Retrieve the currently set singular/plural translation function
     *
     * Returns:
     *   the translation function previously set with <set_ngettext()>, or null
     *   if none is currently set.
     */
    get_ngettext: function () {
        return this._ngettext;
    },

    /**
     * Method: translate_html
     * Translate the HTML page in a webview asynchronously
     *
     * Parameters:
     *   webview - a *WebKit2.WebView* with HTML loaded
     *   cancellable - a *Gio.Cancellable*, or null
     *   callback - a function that takes two parameters: this <WebHelper>
     *     object, and a result object; or null if you don't want a callback
     *
     * Perform translation on all HTML elements marked with name="translatable"
     * in the HTML document displaying in _webview_.
     * The translation will be performed using the function you have set using
     * <set_gettext()>.
     *
     * When the translation is finished, _callback_ will be called.
     * You can get the result of the operation by calling
     * <translate_html_finish()> with the _result_ object passed to _callback_.
     *
     * You can optionally pass _cancellable_ if you want to be able to cancel
     * the operation.
     *
     * Example:
     * > webview.connect('load-changed', (webview, event) => {
     * >     if (event === WebKit2.LoadEvent.FINISHED)
     * >         webhelper.translate_html(webview, null, (obj, res) => {
     * >             webhelper.translate_html_finish(res);
     * >             // Translation done, show the page
     * >             webview.show_all();
     * >         });
     * > });
     */
    translate_html: function (webview, cancellable, callback) {
        let task = { callback: callback };
        // Wait for the DBus interface to appear on the bus
        task.watch_id = Gio.DBus.watch_name(Gio.BusType.SESSION,
            this._extension_name, Gio.BusNameWatcherFlags.NONE,
            (connection) => {
                // name appeared
                let webview_object_path = DBUS_WEBVIEW_EXPORT_PATH +
                    webview.get_page_id();
                // Warning: this._ProxyConstructor will do a synchronous
                // operation unless you pass in a callback
                new this._ProxyConstructor(connection, this._extension_name,
                    webview_object_path, (proxy, error) =>
                {
                    if (error) {
                        this._translate_callback(task, null, error);
                        return;
                    }
                    if (cancellable)
                        proxy.TranslateRemote(cancellable,
                            this._translate_callback.bind(this, task));
                    else
                        proxy.TranslateRemote(this._translate_callback.bind(this,
                            task));
                }, cancellable);
            },
            null);  // do nothing when name vanishes
        return task;
    },

    _translate_callback: function (task, result, error) {
        Gio.DBus.unwatch_name(task.watch_id);
        if (error)
            task.error = error;
        if (task.callback)
            task.callback(this, task);
    },

    /**
     * Method: translate_html_finish
     * Get the result of <translate_html()>
     *
     * Parameters:
     *   res - result object passed to your callback
     *
     * Finishes an operation started by <translate_html()>.
     * If the operation ended in an error, this function will throw that error.
     */
    translate_html_finish: function (res) {
        if (res.error)
            throw res.error;
    },

    /**
     * Method: define_web_action
     * Define an action that may be invoked from a WebView
     *
     * Parameters:
     *   name - a string, which must be a valid URI location.
     *   implementation - a function (see Callback Parameters below.)
     *
     * Callback Parameters:
     *   dict - object containing properties corresponding to the query
     *     parameters that the web action was called with
     *
     * Sets up an action that may be invoked from an HTML document inside a
     * WebView, or from the in-browser Javascript environment inside a WebView.
     * If you set up an action "setVolume" as follows:
     * > app.define_web_action('setVolume', function(dict) { ... });
     * Then you can invoke the action inside the HTML document, e.g. as the
     * target of a link, as follows:
     * > <a href="endless://setVolume?volume=11">This one goes to 11</a>
     * Or from the in-browser Javascript, by navigating to the action URI, as
     * follows:
     * > window.location.href = 'endless://setVolume?volume=11';
     *
     * In both cases, the function would then be called with the _dict_
     * parameter equal to
     * > { "volume": "11" }
     *
     * If an action called _name_ is already defined, the new _implementation_
     * replaces the old one.
     */
    define_web_action: function (name, implementation) {
        if (typeof implementation !== 'function') {
            throw new Error('The implementation of a web action must be a ' +
                'function.');
        }
        this._web_actions[name] = implementation;
    },

    /**
     * Method: define_web_actions
     * Define several web actions at once
     *
     * Parameters:
     *   dict - an object, with web action names as property names, and their
     *     implementations as values
     *
     * Convenience method to define more than one web action at once.
     * Calls <define_web_action()> on each property of _dict_.
     *
     * *Note* This API is Javascript-only. It will not be implemented in C.
     */
    define_web_actions: function (dict) {
        Object.keys(dict).forEach((key) =>
            this.define_web_action(key, dict[key]));
    },

    /**
     * Method: unregister
     * Break the connection to WebKit
     *
     * Breaks the connection to all webviews and removes all DBus objects.
     * You should call this in your application's *vfunc_dbus_unregister()*
     * implementation.
     *
     * After this function has been called, no WebHelper functionality will
     * work.
     */
    unregister: function () {
        this._dbus_impl.unexport_from_connection(this.connection);
    },
});
