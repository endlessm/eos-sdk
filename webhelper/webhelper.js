const Endless = imports.gi.Endless;
const GLib = imports.gi.GLib;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;
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
 * The custom URI corresponds to a function that you define using
 * <Application.define_web_action()>, and you can pass parameters to the
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

    _init: function(props) {
        this._webActions = {};
        this._translationFunction = null;
        this.parent(props);
    },

    /**
     * Method: define_web_action
     * Define an action that may be invoked from a WebView
     *
     * Parameters:
     *   - name: a string, which must be a valid URI location.
     *   - implementation: a function (see Callback Parameters below.)
     *
     * Callback Parameters:
     *   - dict: object containing properties corresponding to the query
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
     * > window.location = 'endless://setVolume?volume=11'
     *
     * In both cases, the function would then be called with the _dict_
     * parameter equal to
     * > { "volume": "11" }
     *
     * If an action called _name_ is already defined, the new _implementation_
     * replaces the old one.
     */
    define_web_action: function(name, implementation) {
        if(typeof implementation != 'function') {
            throw new Error('The implementation of a web action must be a ' +
                'function.');
        }
        this._webActions[name] = implementation;
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
    define_web_actions: function(dict) {
        for(let key in dict) {
            if(dict.hasOwnProperty(key)) {
                this.define_web_action(key, dict[key]);
            }
        }
    },

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
            throw new Error("Undefined WebHelper action '%s'. Did you define " +
                "it with WebHelper.Application.define_web_action()?".format(
                    function_name));

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
     * Method: set_translation_function
     * Define function which transforms all translatable text
     *
     * Parameters:
     *   translation_function - a function, or null
     *
     * When you plan to use the <translate_html()> function to translate text in
     * your web application, set this property to the translation function.
     * The function must take one parameter, a string, and also return a
     * string.
     * The canonical example is gettext().
     *
     * Pass null for _translation_function_ to unset the translation function.
     *
     * If this function has not been called, or has most recently been called
     * with null, then it is illegal to call <translate_html()>.
     */
    set_translation_function: function(translation_function) {
        if(translation_function !== null
            && typeof translation_function !== 'function') {
            throw new Error('The translation function must be a function, or ' +
                'null.');
        }
        this._translationFunction = translation_function;
    },

    /**
     * Method: get_translation_function
     * Retrieve the currently set translation function
     *
     * Returns:
     *   the translation function previously set with
     *   <set_translation_function()>, or null if none is currently set.
     */
    get_translation_function: function() {
        return this._translationFunction;
    },

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
     * you specify using <set_translation_function()> property.
     * The return value from the translation function will be substituted into
     * the HTML instead of the original string.
     *
     * Example:
     * > app.set_translation_function(_);
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
                    "Did you forget to call 'set_translation_function()' on " +
                    "your app?");
            element.inner_html = this._translationFunction(element.inner_text);
        }
    }
});
