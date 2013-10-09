const Endless = imports.gi.Endless;
const Gio = imports.gi.Gio;
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
        this.webview.connect('navigation-policy-decision-requested',
            Lang.bind(this, this.web_actions_handler));
        let string = ('<html><head><meta http-equiv="refresh" content="0;url=' +
            this.webActionToTest + '"></head><body></body></html>');
        this.webview.load_string(string, 'text/html', 'UTF-8', 'file://');
        this.win = new Endless.Window({
            application: this
        });
        this.scrolled = new Gtk.ScrolledWindow();
        this.scrolled.add(this.webview);
        this.win.page_manager.add(this.scrolled);

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
    let fake_pid = GLib.random_int();
    // FIXME In this version of GJS there is no Posix module, so fake the PID
    let id_string = 'com.endlessm.webhelper.test' + GLib.get_real_time() + fake_pid;
    app = new TestClass({
        application_id: id_string
    });
}

function testWebActionIsCalled() {
    let actionWasCalled = false;
    app.define_web_action('quitApplication', function() {
        actionWasCalled = true;
        app.quit();
    });
    app.webActionToTest = 'endless://quitApplication';
    app.run([]);
    assertTrue(actionWasCalled);
}

function testWebActionIsCalledWithParameter() {
    let actionParameter;
    app.define_web_action('getParameterAndQuit', function(dict) {
        actionParameter = dict['param'];
        app.quit();
    });
    app.webActionToTest = 'endless://getParameterAndQuit?param=value';
    app.run([]);
    assertEquals('value', actionParameter);
}

function testWebActionIsCalledWithManyParameters() {
    let firstParameter, secondParameter, thirdParameter;
    app.define_web_action('getParametersAndQuit', function(dict) {
        firstParameter = dict['first'];
        secondParameter = dict['second'];
        thirdParameter = dict['third'];
        app.quit();
    });
    app.webActionToTest = 'endless://getParametersAndQuit?first=thefirst&second=thesecond&third=thethird';
    app.run([]);
    assertEquals('thefirst', firstParameter);
    assertEquals('thesecond', secondParameter);
    assertEquals('thethird', thirdParameter);
}

function testParameterNameIsUriDecoded() {
    let expectedParameter = 'päräm💩';
    let parameterWasFound = false;
    app.define_web_action('getUriDecodedParameterAndQuit', function(dict) {
        parameterWasFound = (expectedParameter in dict);
        app.quit();
    });
    app.webActionToTest = 'endless://getUriDecodedParameterAndQuit?p%C3%A4r%C3%A4m%F0%9F%92%A9=value';
    app.run([]);
    assertTrue(parameterWasFound);
}

function testParameterValueIsUriDecoded() {
    let expectedValue = 'válué💩';
    let actualValue;
    app.define_web_action('getUriDecodedValueAndQuit', function(dict) {
        actualValue = dict['param'];
        app.quit();
    });
    app.webActionToTest = 'endless://getUriDecodedValueAndQuit?param=v%C3%A1lu%C3%A9%F0%9F%92%A9';
    app.run([]);
    assertEquals(expectedValue, actualValue);
}

// This is commented out because GJS cannot catch exceptions across FFI
// interfaces (e.g. in GObject callbacks.)
// function testBadActionIsNotCalled() {
//     app.webActionToTest = 'endless://nonexistentAction?param=value';
//     assertRaises(function() { app.run([]); });
// }

function testWebActionIsCalledWithBlankParameter() {
    let parameterWasFound = false;
    let parameterValue;
    app.define_web_action('getBlankValueAndQuit', function(dict) {
        parameterWasFound = ('param' in dict);
        if(parameterWasFound)
            parameterValue = dict['param'];
        app.quit();
    });
    app.webActionToTest = 'endless://getBlankValueAndQuit?param=';
    app.run([]);
    assertTrue(parameterWasFound);
    assertNotUndefined(parameterValue);
    assertEquals('', parameterValue);
}

function testWebActionIsUriDecoded() {
    let actionWasCalled = false;
    app.define_web_action('äction💩Quit', function(dict) {
        actionWasCalled = true;
        app.quit();
    });
    app.webActionToTest = 'endless://%C3%A4ction%F0%9F%92%A9Quit';
    app.run([]);
    assertTrue(actionWasCalled);
}

function testDefineMultipleActionsOverride() {
    let actionWasCalled = false;
    app.define_web_actions({
        quitApplication: function() {
            actionWasCalled = true;
            app.quit();
        }
    });
    app.webActionToTest = 'endless://quitApplication';
    app.run([]);
    assertTrue(actionWasCalled);
}

function testDefineBadAction() {
    assertRaises(function() {
        app.define_web_action('badAction', 'not a function');
    });
}
