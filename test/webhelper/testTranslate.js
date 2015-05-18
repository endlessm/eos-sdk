const Endless = imports.gi.Endless;
const GLib = imports.gi.GLib;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;
const WebHelper = imports.webhelper;
const WebKit = imports.gi.WebKit;

const WebHelperApplicationWithTranslatableText = new Lang.Class({
    Name: 'WebHelperApplicationWithTranslatableText',
    Extends: WebHelper.Application,
    
    get_translation_string: function() {
        return 'Translate Me';
    },

    vfunc_startup: function() {
        this.parent();
        this.webview = new WebKit.WebView();
        let string = '<html><body><p name="translatable">' +
                     this.get_translation_string() +
                     '</p></body></html>';
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
    }
});

describe("Translation strategy", function() {
    let app;

    beforeEach(function() {
        // FIXME In this version of GJS there is no Posix module, so fake the PID
        let id_string = 'com.endlessm.webhelper.test' + GLib.get_real_time() + fake_pid;
        // Generate a unique ID for each app instance that we test
        let fake_pid = GLib.random_int();
        app = new WebHelperApplicationWithTranslatableText({
            application_id: id_string
        });
    });
 
    describe("translation function", function() {
        let translationFunctionSpy;
        beforeEach(function() {
            translationFunctionSpy = jasmine.createSpy('translate').and.returnValue('Translated');
        });
        it("gets called with string to translate on run", function() {
            app.set_translation_function(translationFunctionSpy);
            app.run([]);
            expect(translationFunctionSpy).toHaveBeenCalledWith(app.get_translation_string());
        });
    });

    it("throws when an incompatible type is set as the translation function", function() {
        expect(function() {
            app.set_translation_function({});
        }).toThrow();
    });

    // Can't test this right now as there is no support for propagating exceptions across
    // GI interfaces
    xit("throws when there isn't a translation function set", function() {
        expect(function() {
            app.run([]);
        }).toThrow();
    });

    it("has a null translation function by default", function() {
        expect(app.get_translation_function()).toBe(null);
    });

    it("stores the expected translation function", function() {
        let translation = function(str) {
            return str;
        };
        
        app.set_translation_function(translation);
        expect(app.get_translation_function()).toBe(translation);
    });

    it("allows us to store a null translation function", function() {
        let nonNullTranslation = function(str) {
            return str;
        }
        
        // set a non-null translation function first so that we get
        // the non-default behaviour for get_translation_function
        app.set_translation_function(nonNullTranslation);
        app.set_translation_function(null);
        expect(app.get_translation_function()).toBe(null);
    });
});
