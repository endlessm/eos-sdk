const Endless = imports.gi.Endless;
const GLib = imports.gi.GLib;
const Gtk = imports.gi.Gtk;
const WebKit = imports.gi.WebKit;

const EOS_URI_SCHEME = 'endless://';

/**
 * Namespace: WebHelper
 * Convenience library for running web applications
 *
 * WebHelper is a convenience library for displaying web applications inside a
 * GTK container running WebKitGTK (currently WebKit 1, not 2.)
 * Although WebKit provides an easy way of communicating from GTK code to
 * the in-browser Javascript, through the execute_script() method, it is not so
 * easy to communicate the other way around.
 *
 * WebHelper solves that problem by detecting when the web page navigates to a
 * custom action URI.
 * The custom URI corresponds to a function that you define in
 * <Application._webActions>, and you can pass parameters to the
 * function.
 *
 * Another often-encountered problem is localizing text through the same API as
 * your main GTK program.
 * WebHelper solves this problem by allowing you to mark strings in your HTML
 * page and translating them through a function of your choice when you run
 * <Application.translate_html()>.
 */

/**
 * Class: Application
 * Main application class for a WebHelper application
 *
 * The application class in GJS for your web application should extend
 * WebHelper.Application.
 *
 * You should set up any WebViews that you create, by connecting
 * <web_actions_handler()>, so that they can handle custom action URIs.
 */
const Application = new Lang.Class({
    Name: 'WebApplication',
    Extends: Endless.Application,

    /**
     * Property: _webActions
     * Set of actions that may be invoked from a WebView
     *
     * Declare them as a function that takes a dict as a parameter, and use
     * links with the format "endless://actionName?parameter=value"
     *
     * *Note* This API will likely disappear and be replaced by a method
     * add_web_action().
     * That is the reason for the underscore starting the name.
     */
    _webActions: { },

    /**
     * Callback: web_actions_handler
     * Navigation callback which routes the custom URIs to actions
     *
     * When you create a web view in which you want to run a web application
     * that uses custom action URIs, remember to set it up by connecting this
     * callback to its "navigation-policy-decision-requested" signal. The
     * following code will do this:
     *
     * > webview.connect('navigation-policy-decision-requested',
     * >     Lang.bind(app, app.web_actions_handler));
     *
     * where 'webview' is the web view, and 'app' is the WebHelper.Application
     * instance.
     */
    web_actions_handler: function(webview, frame, request, action, policy_decision) {
        let uri = request.get_uri();

        if(uri.indexOf(EOS_URI_SCHEME) !== 0) {
            // this is a regular URL, just navigate there
            return false;
        }

        // get the name and parameters for the desired function
        let f_call = uri.substring(EOS_URI_SCHEME.length, uri.length).split('?');
        var function_name = f_call[0];
        var parameters = {};

        if(f_call[1]) {
            // there are parameters
            let params = f_call[1].split('&');
            params.forEach(function(entry) {
                let param = entry.split('=');

                if(param.length == 2) {
                    param[0] = decodeURIComponent(param[0]);
                    param[1] = decodeURIComponent(param[1]);
                    // and now we add it...
                    parameters[param[0]] = param[1];
                }
            });
        }

        if(this._webActions[function_name])
            Lang.bind(this, this._webActions[function_name])(parameters);
        else
            throw new Error("Undefined WebHelper action '%s'. Did you add it " +
                "to your app's _webActions object?".format(function_name));

        policy_decision.ignore();
        return true;
    },

//  convenience functions

    _getElementById: function(webview, id) {
        // WebKit.DOMDocument
        let dom = webview.get_dom_document();

        // WebKit.DOMElement
        return dom.get_element_by_id(id);
    },

    /**
     * Property: _translationFunction
     * Function which transforms all translatable text
     *
     * When you plan to use the <translate_html()> function to translate text in
     * your web application, set this property to the translation function.
     * The function must take one parameter, a string, and also return a
     * string -- for example, gettext().
     *
     * *Note* This API will likely disappear and be replaced by a read-write
     * translation_function property.
     * That is the reason for the underscore starting the name.
     */

    /**
     * Method: translate_html
     * Translate all translatable text currently showing in a web view
     *
     * Parameters:
     *   webview - the WebView containing the text to translate.
     *
     * Another problem with running web applications inside a GTK container is
     * keeping all the localization data in the same place.
     * For example, the most obvious way to do localization in a GTK application
     * is to use gettext(), but that does not work so easily inside a web view.
     *
     * In a <WebHelper.Application>, you can mark strings in your HTML for
     * translation by enclosing them in
     * > <span name="translatable">String to be translated</span>
     * or just putting the "translatable" name directly on the element
     * containing the string, e.g. <p> or <h1>.
     *
     * Then after the web view has finished loading, call <translate_html()> on
     * it, and each of the marked strings will be passed to the function that
     * you specify using the <_translationFunction> property.
     * The return value from <_translationFunction> will be substituted into the
     * HTML instead of the original string.
     *
     * Example:
     * > app._translationFunction = _;
     * > webview.connect('notify::load-status',
     * >     Lang.bind(app, function(webview) {
     * >         if(webview.load_status == WebKit.LoadStatus.FINISHED)
     * >             this.translate_html(webview);
     * >     }));
     */
    translate_html: function(webview) {
        let dom = webview.get_dom_document();

        // WebKit.DOMNodeList
        let translatable = dom.get_elements_by_name('translatable');

        for (var i = 0 ; i < translatable.get_length() ; i++) {
            // WebKit.DOMNode
            let element = translatable.item(i);

            // Translate the text
            if(typeof this._translationFunction !== 'function')
                throw new Error("No suitable translation function was found. " +
                    "Did you forget to set '_translationFunction' on your app?");
            element.inner_html = this._translationFunction(element.inner_text);
        }
    }
});
