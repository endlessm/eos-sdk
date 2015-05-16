// Copyright 2015 Endless Mobile, Inc.

imports.gi.versions.WebKit2 = '4.0';

const Gio = imports.gi.Gio;
const GLib = imports.gi.GLib;
const GObject = imports.gi.GObject;
const Lang = imports.lang;
const WebKit2 = imports.gi.WebKit2;

const Config = imports.webhelper_private.config;

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
 * One often-encountered problem is localizing text through the same API as
 * your main GTK program.
 * WebHelper solves this problem by allowing you to mark strings in your HTML
 * page and translating them through a function of your choice when you run
 * <WebHelper.translate_html()>.
 */

/**
 * Class: WebHelper
 * Helper object for a WebKit2 web application
 *
 * Constructor parameters:
 *   props - a dictionary of construction properties and values (default {})
 *
 * The application class for your web application should create <WebHelper> in
 * its *vfunc_dbus_register()* implementation, with appropriate
 * <well-known-name> and <connection> parameters.
 * After that, you can do further setup on it, such as defining web actions, in
 * your *vfunc_startup()* implementation.
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
        this._gettext = null;
        this._ProxyConstructor =
            Gio.DBusProxy.makeProxyWrapper(WH2_DBUS_EXTENSION_INTERFACE);
        this.parent(props);

        if (this.well_known_name === '')
            throw new Error('The "well-known-name" parameter is required.');
        this._extension_name = this.well_known_name + '.webhelper';

        // Set up Webkit to load our web extension
        let context = WebKit2.WebContext.get_default();
        context.connect('initialize-web-extensions', () => {
            let libexec = Gio.File.new_for_path(Config.LIBEXECDIR);
            let path = libexec.get_child('webhelper2').get_path();

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
    },

    // DBus implementations

    Gettext: function (string) {
        return this._gettext(string);
    },

    // Public API

    /**
     * Method: set_gettext
     * Define function which translates text
     *
     * Parameters:
     *   gettext_func - a function, or null
     *
     * When you plan to use the <translate_html()> function to translate text in
     * your web application, set this property to the translation function.
     * The function must take one parameter, a string, and also return a
     * string.
     * The canonical example is gettext().
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
            (connection, name, owner) => {
                // name appeared
                let webview_object_path = DBUS_WEBVIEW_EXPORT_PATH +
                    webview.get_page_id();
                let proxy = new this._ProxyConstructor(connection,
                    this._extension_name, webview_object_path);
                if (cancellable)
                    proxy.TranslateRemote(cancellable,
                        this._translate_callback.bind(this, task));
                else
                    proxy.TranslateRemote(this._translate_callback.bind(this,
                        task));
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
