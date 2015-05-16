// Copyright 2015 Endless Mobile, Inc.

const Endless = imports.gi.Endless;
const Gettext = imports.gettext;
const Lang = imports.lang;
const WebHelper2 = imports.webhelper2;
const WebKit2 = imports.gi.WebKit2;

const TEST_APPLICATION_ID = 'com.endlessm.example.test-webview2';
const TEST_HTML = '\
<html> \
<head> \
<title>First page</title> \
<style> \
p, form { \
    width: 50%; \
    padding: 1em; \
    background: #FFFFFF; \
} \
body { \
    background: #EEEEEE; \
} \
</style> \
</head> \
\
<body> \
<h1>First page</h1> \
\
<p><a href="http://wikipedia.org">Regular link to a Web site</a></p> \
\
<p>This is text that will be italicized: <span name="translatable">Hello, \
world!</span></p> \
\
</body> \
</html>';

const TestApplication = new Lang.Class({
    Name: 'TestApplication',
    Extends: Endless.Application,

    vfunc_dbus_register: function (connection, object_path) {
        this._webhelper = new WebHelper2.WebHelper({
            well_known_name: this.application_id,
            connection: connection,
        });
        return this.parent(connection, object_path);
    },

    vfunc_startup: function () {
        this.parent();

        this._webhelper.set_gettext((s) => s.italics());

        this._webview = new WebKit2.WebView();
        this._webview.connect('load-changed', (webview, event) => {
            if (event === WebKit2.LoadEvent.FINISHED)
                this._webhelper.translate_html(webview, null, (obj, res) => {
                    this._webhelper.translate_html_finish(res);
                });
        });
        this._webview.load_html(TEST_HTML, 'file://');

        this._window = new Endless.Window({
            application: this,
            border_width: 16,
        });

        this._pm = this._window.page_manager;
        this._pm.add(this._webview, { name: 'page1' });

        this._window.show_all();
    },

    vfunc_dbus_unregister: function (connection, object_path) {
        this.parent(connection, object_path);
        this._webhelper.unregister();
    },
});

let app = new TestApplication({
    application_id: TEST_APPLICATION_ID,
});
app.run(ARGV);
