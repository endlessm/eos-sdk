const Endless = imports.gi.Endless;
const Gio = imports.gi.Gio;
const GLib = imports.gi.GLib;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;
const WebHelper = imports.webhelper;
const WebKit = imports.gi.WebKit;

const WebUpdateFontSizeTestApplication = new Lang.Class({
    Name: 'WebUpdateFontSizeTestApplication',
    Extends: WebHelper.Application,

    vfunc_startup: function () {
        this.parent();

        this.webview = new WebKit.WebView();
        this.websettings = this.webview.get_settings();
        this.webview.set_settings(this.websettings);
        this.win = new Endless.Window({
            application: this,
            font_scaling_active: true
        });

        /* Typically logic in tests is bad. In this case, we need to inject these
         * callbacks to test specific cases that only occur during the runtime main
         * loop of the test application. */
        if (this.set_font_resizable_callback !== null) {
            this.set_font_resizable_callback(this.win, this.websettings);
            this.websettings.connect('notify::default-font-size', Lang.bind(this, function () {
                this.default_font_size_changed_callback(this.win, this.websettings);
            }));
        }

        if (this.accept_callback !== null)
            this.accept_callback(this.win, this.websettings.default_font_size);
    },

    accept_callback: null,

    set_font_resizable_callback: null,

    default_font_size_changed_callback: null
});

// TODO: These tests depend on a running X Server and Window Manager. That means
// that they are not runnable in a continuous-integration server
describe("Web Helper Font Sizes", function () {
    let app;

    beforeEach(function () {
        // Generate a unique ID for each app instance that we test
        let fake_pid = GLib.random_int();
        // FIXME In this version of GJS there is no Posix module, so fake the PID
        let id_string = 'com.endlessm.webhelper.test' + GLib.get_real_time() + fake_pid;

        app = new WebUpdateFontSizeTestApplication({
            application_id: id_string
        });
    });
    
    it("does not have the default font scale for a tiny window", function () {
        let test_initial_font_size = function (my_win, default_font_size) {
            my_win.font_scaling_default_window_size = 200; // Force a ridiculous value
            // Window's font scaling is a double, web settings' is an integer
            expect(my_win.font_scaling_calculated_font_size).not.toBeCloseTo(default_font_size, 0);

            /* This does not immediately quit the application. It sets a flag for termination
             * which will cause the application to be disposed on the next main loop iteration. */
            app.quit();
        };

        app.accept_callback = test_initial_font_size;

        spyOn(app, 'accept_callback').and.callThrough();
        app.run([]);
        expect(app.accept_callback).toHaveBeenCalled();
    });
    
    it("takes the provided settings default font size on setting resizable", function () {
        app.set_font_resizable_callback = app.set_web_settings_font_resizable;

        let test_font_sizing = function (my_win, my_websettings) {
            // Window's font scaling is a double, web settings' is an integer
            expect(my_win.font_scaling_calculated_font_size).toBeCloseTo(my_websettings.default_font_size, 0);

            /* This does not immediately quit the application. It sets a flag for termination
             * which will cause the application to be disposed on the next main loop iteration. */
            app.quit();
        };

        app.default_font_size_changed_callback = test_font_sizing;

        spyOn(app, 'default_font_size_changed_callback').and.callThrough();
        app.run([]);
        expect(app.default_font_size_changed_callback).toHaveBeenCalled();
    });
});
