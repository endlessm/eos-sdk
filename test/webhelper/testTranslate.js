const Endless = imports.gi.Endless;
const GLib = imports.gi.GLib;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;
const WebHelper = imports.webhelper;
const WebKit = imports.gi.WebKit;

const TestClass = new Lang.Class({
    Name: 'testclass',
    Extends: WebHelper.Application,

    vfunc_startup: function() {
        this.parent();
        this.webview = new WebKit.WebView();
        let string = '<html><body><p name="translatable">Translate Me</p></body></html>';
        this.webview.load_string(string, 'text/html', 'UTF-8', 'file://');
        this.win = new Endless.Window({
            application: this
        });
        this.scrolled = new Gtk.ScrolledWindow();
        this.scrolled.add(this.webview);
        this.win.page_manager.add(this.scrolled);

        this.webview.connect('notify::load-status', Lang.bind(this, function() {
            if(this.webview.load_status === WebKit.LoadStatus.FINISHED) {
                this.translate_html(this.webview);
                this.quit();
            }
        }));

        // Add an upper bound on how long the app runs, in case app.quit() does
        // not get called
        GLib.timeout_add_seconds(GLib.PRIORITY_HIGH, 5, Lang.bind(this, function() {
            this.quit();
        }));
    }
});

let app;

function setUp() {
    // Generate a unique ID for each app instance that we test
    let id_string = 'com.endlessm.webhelper.test' + GLib.get_real_time();
    app = new TestClass({
        application_id: id_string
    });
}

function testStringIsTranslated() {
    let translationFunctionWasCalled = false;
    let translationFunctionCalledWithString;
    app.set_translation_function(function(s) {
        translationFunctionWasCalled = true;
        translationFunctionCalledWithString = s;
        return s;
    });
    app.run([]);
    assertTrue(translationFunctionWasCalled);
    assertEquals('Translate Me', translationFunctionCalledWithString);
}

// The following test is commented out because GJS cannot catch exceptions
// across FFI interfaces (e.g. in GObject callbacks.)

// function testMissingTranslationFunctionIsHandled() {
//     assertRaises(function() {
//         app.run([]);
//     });
// }

function testSetBadTranslationFunction() {
    assertRaises(function() {
        app.set_translation_function("I am not a function");
    });
}

function testGetSetTranslationFunction() {
    let translationFunction = function(string) {
        return string;
    };
    app.set_translation_function(translationFunction);
    let actualTranslationFunction = app.get_translation_function();
    assertEquals(translationFunction, actualTranslationFunction);
}

function testTranslationFunctionIsNullByDefault() {
    assertNull(app.get_translation_function());
}

function testGetSetNullTranslationFunction() {
    app.set_translation_function(function (s) { return s; });
    assertNotNull(app.get_translation_function());
    app.set_translation_function(null);
    assertNull(app.get_translation_function());
}
