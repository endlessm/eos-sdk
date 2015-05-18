const Endless = imports.gi.Endless;
const Gio = imports.gi.Gio;
const GLib = imports.gi.GLib;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;
const WebHelper = imports.webhelper;
const WebKit = imports.gi.WebKit;

const WebActionTestApplication = new Lang.Class({
    Name: 'WebActionTestApplication',
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
    }
});

// TODO: These tests depend on a running X Server and Window Manager. That means
// that they are not runnable in a continuous-integration server
describe("Web Actions Bindings", function() {
    let app;
    let webActionSpy;
    
    beforeEach(function() {
        // Generate a unique ID for each app instance that we test
        let fake_pid = GLib.random_int();
        // FIXME In this version of GJS there is no Posix module, so fake the PID
        let id_string = 'com.endlessm.webhelper.test' + GLib.get_real_time() + fake_pid;
        app = new WebActionTestApplication({
            application_id: id_string
        });
        webActionSpy = jasmine.createSpy('quitAction').and.callFake(function() {
            // Calls destroy on the applications window, which decrements the hold count on the
            // application and implicitly causes the application to close.
            app.win.destroy();
        });
    });
    
    let RunApplicationWithWebAction = function(app, action) {
        app.webActionToTest = action;
        app.run([]);
    }
    it("has a working quitApplication uri upon defining quitApplication as a string", function() {
        app.define_web_action('quitApplication', webActionSpy);
        RunApplicationWithWebAction(app, 'endless://quitApplication');

        expect(webActionSpy).toHaveBeenCalled();
    });

    it("is called with a parameter", function() {
        app.define_web_action('getParameterAndQuit', webActionSpy);
        RunApplicationWithWebAction(app, 'endless://getParameterAndQuit?param=value');

        expect(webActionSpy).toHaveBeenCalledWith(new jasmine.ObjectContaining({ param: 'value' }));
    });

    it("can be called with many parameters", function() {
        app.define_web_action('getParametersAndQuit', webActionSpy);
        RunApplicationWithWebAction(app, 'endless://getParametersAndQuit?first=thefirst&second=thesecond&third=thethird');

        expect(webActionSpy).toHaveBeenCalledWith(new jasmine.ObjectContaining({
            first: 'thefirst',
            second: 'thesecond',
            third: 'thethird'
        }));
    });

    it("decodes parameter URI names", function() {
        app.define_web_action('getUriDecodedParameterAndQuit', webActionSpy);
        RunApplicationWithWebAction(app, 'endless://getUriDecodedParameterAndQuit?p%C3%A4r%C3%A4m%F0%9F%92%A9=value');

        expect(webActionSpy).toHaveBeenCalledWith(new jasmine.ObjectContaining({
            'päräm💩' : 'value'
        }));
    });

    it("decodes parameter URI values", function() {
        app.define_web_action('getUriDecodedParameterValueAndQuit', webActionSpy);
        RunApplicationWithWebAction(app, 'endless://getUriDecodedParameterValueAndQuit?param=v%C3%A1lu%C3%A9%F0%9F%92%A9');

        expect(webActionSpy).toHaveBeenCalledWith(new jasmine.ObjectContaining({
            param : 'válué💩'
        }));
    });

    // We currently can't catch exceptions across GObject-Introspection callbacks
    xit('bad action is not called', function() {
        expect(function() { RunApplicationWithWebAction(app, 'endless://nonexistentWebAction') }).toThrow();
    });

    describe("with blank parameters", function() {
        beforeEach(function() {
            app.define_web_action('getBlankValueAndQuit', webActionSpy);
            RunApplicationWithWebAction(app, 'endless://getBlankValueAndQuit?param=');
        });

        it("can be called", function() {
            expect(webActionSpy).toHaveBeenCalled();
        });

        it("is called with a paramater that is an empty string", function() {
            expect(webActionSpy).toHaveBeenCalledWith(new jasmine.ObjectContaining({ 
                'param' : ''
            }));
        });
    });

    it("URI decodes the action", function() {
        app.define_web_action('äction💩Quit', webActionSpy);
        RunApplicationWithWebAction(app, 'endless://%C3%A4ction%F0%9F%92%A9Quit');
        expect(webActionSpy).toHaveBeenCalled();
    });

    it("allows web actions to be defined as object properties", function() {
        app.define_web_actions({
            quitApplication: webActionSpy
        });
 
        RunApplicationWithWebAction(app, 'endless://quitApplication');

        expect(webActionSpy).toHaveBeenCalled();
    });

    it("throws an error when trying to define an action that is not a function", function() {
        expect(function() {
            app.define_web_action('action', {});
        }).toThrow();
    });
});
