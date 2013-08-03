const EndlessWikipedia = imports.wikipedia.EndlessWikipedia;
const Gettext = imports.gettext;
const Lang = imports.lang;
const GObject = imports.gi.GObject;
const Gtk = imports.gi.Gtk;
const Endless = imports.gi.Endless;

const BackButton = imports.BackButton;
const SideBarButton = imports.SideBarButton;

const _ = Gettext.gettext;

const SIDEBAR_BACK_BUTTON_URI = "/com/endlessm/wikipedia-domain/assets/image_strip_back_button.png";
const SUBMENU_SEPARATOR_A_URI = "/com/endlessm/brazil/assets/submenu_separator_shadow_a.png";
const SUBMENU_SEPARATOR_B_URI = "/com/endlessm/brazil/assets/submenu_separator_shadow_b.png";

const DomainWikiView = new Lang.Class({
    Name: "DomainWikiView",
    Extends: GObject.Object,
    Signals: {
        'category-chosen': {
            param_types: [GObject.TYPE_STRING, GObject.TYPE_INT]
        },
        'article-chosen': {
            param_types: [GObject.TYPE_STRING, GObject.TYPE_INT]
        }
    },

    _init: function(application) {
        this.parent();
        this._presenter = null;

        this._window = new Endless.Window({
            application: application,
            title: _("Endless Domain Wikis")
        });

        // These need to be called first
        this.create_front_page();
        this.create_category_page();
        this.create_article_page();

        this._window.page_manager.transition_duration = 200;  // ms
        this._window.page_manager.add(this._front_page, {
            name: 'front'
        });

        this._window.page_manager.add(this._category_page, {
            name: 'category',
            left_topbar_widget: this._category_back_button
        });
        
        this._window.page_manager.add(this._article_page, {
            name: 'article',
            left_topbar_widget: this._article_back_button
        })
     
        this._window.show_all();
    },

    create_front_page: function(){
        this._front_page = new EndlessWikipedia.PrebuiltFrontPage();
        this._front_page.connect('category-chosen',
            Lang.bind(this, this._onCategoryClicked));
    },

    create_article_page: function(){
        // Article page
        this._article_page = new Gtk.Grid({
            orientation: Gtk.Orientation.HORIZONTAL
        });

        this._sidebar_frame = new Gtk.Frame({
            name: "sidebar_frame"
        });
        this._sidebar_frame.set_size_request(40, -1);

        this._article_sidebar_back_button = new SideBarButton.SideBarButton(SIDEBAR_BACK_BUTTON_URI, {
            name: "side_bar_button",
            vexpand: true
        });
        this._article_view = new EndlessWikipedia.PrebuiltArticlesPage();
        this._article_list = new EndlessWikipedia.ArticleList({
            halign: Gtk.Align.START,
            hexpand: false
        });

        this._submenu_separator_a = new Gtk.Image({
            halign: Gtk.Align.END,
            resource: SUBMENU_SEPARATOR_A_URI
        });

        this._submenu_separator_b = new Gtk.Image({
            halign: Gtk.Align.START,
            resource: SUBMENU_SEPARATOR_B_URI
        });

        this._overlay_left = new Gtk.Overlay();
        this._overlay_left.add(this._sidebar_frame);
        this._overlay_left.add_overlay(this._submenu_separator_a);
        this._overlay_left.add_overlay(this._article_sidebar_back_button);

        this._overlay_right = new Gtk.Overlay();
        this._overlay_right.add(this._article_view);
        this._overlay_right.add_overlay(this._submenu_separator_b); 

        this._article_page.add(this._overlay_left);

        this._article_page.add(this._article_list);

        this._article_page.add(this._overlay_right);

        this._article_back_button = new BackButton.BackButton();
        this._article_back_button.show();

        this._article_list.connect('article-chosen',
            Lang.bind(this, this._onArticleClicked));
        this._article_back_button.connect('clicked',
            Lang.bind(this, this._onArticleBackClicked));
        this._article_sidebar_back_button.connect('clicked', Lang.bind(this, function() {
            this._onArticleBackClicked();
        }));
    },

    create_category_page: function(){
        // Category page
        this._category_page = new Gtk.Grid({
            orientation: Gtk.Orientation.HORIZONTAL
        });
        this._category_view = new EndlessWikipedia.PrebuiltCategoryPage();
        // _category_article_list is eventually going to be the same widget as
        // _article_list, so that's why it's not built into the
        // PrebuiltCategoryPage
        this._category_article_list = new EndlessWikipedia.ArticleList({
            halign: Gtk.Align.END,
            hexpand: false
        });
        this._category_page.add(this._category_view);
        this._category_page.add(this._category_article_list);

        this._category_back_button = new BackButton.BackButton({
            label: _('START')
        });
        this._category_back_button.show();


        this._category_article_list.connect('article-chosen',
            Lang.bind(this, this._onArticleClicked));

        this._category_back_button.connect('clicked',
            Lang.bind(this, this._onCategoryBackClicked));
    },

    _set_article_sidebar_uri: function(uri){
        let frame_css = "#sidebar_frame{background-image: url('" + uri + "');background-repeat:no-repeat;background-size:cover;}";
        let provider = new Gtk.CssProvider();
        provider.load_from_data(frame_css);
        let context = this._sidebar_frame.get_style_context();
        context.add_provider(provider, Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION);
    },

    set_presenter: function(presenter) {
        this._presenter = presenter;
    },

    set_front_page_info: function(title, image_uri) {
        this._front_page.title = title;
        this._front_page.image_uri = image_uri;
    },

    set_category_info: function(category, articles) {
        this._category_view.title = category.title;
        this._category_view.description = category.description;
        this._category_view.image_uri = category.image_uri;

        this._set_article_sidebar_uri(category.image_uri);

        this._category_article_list.setArticles(articles);
        this._article_list.setArticles(articles);
        this._article_back_button.label = category.title.toUpperCase();
    },

    set_article_info: function(article){
        this._article_view.article_title = article.title;
    },

    transition_page: function(transition_type, page_name){
        this._window.page_manager.transition_type = transition_type;
        this._window.page_manager.visible_page_name = page_name;
    },

    set_categories: function(categories){
        this._front_page.setCategories(categories);
    },

    _onCategoryClicked: function(page, title, index) {
        this.emit('category-chosen', title, index);
    },

    _onArticleClicked: function(article_list, title, index) {
        this.emit('article-chosen', title, index);
    },

    _onCategoryBackClicked: function(button) {
        this._window.page_manager.transition_type = Endless.PageManagerTransitionType.SLIDE_RIGHT;
        this._window.page_manager.visible_page_name = 'front';
    },

    _onArticleBackClicked: function(button) {
        this._window.page_manager.transition_type = Endless.PageManagerTransitionType.SLIDE_RIGHT;
        this._window.page_manager.visible_page_name = 'category';
    }
});
