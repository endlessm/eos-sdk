const Endless = imports.gi.Endless;
const Format = imports.format;
const Lang = imports.lang;
const Gtk = imports.gi.Gtk;

const EndlessWikipedia = imports.wikipedia.EndlessWikipedia;
const DomainWikiView = imports.wikipedia.views.domain_wiki_view;
const DomainWikiModel = imports.wikipedia.models.domain_wiki_model;
const DomainWikiPresenter = imports.wikipedia.presenters.domain_wiki_presenter;

String.prototype.format = Format.format;

const PrebuiltWikipediaApplication = new Lang.Class({
    Name: 'PrebuiltWikipediaApplication',
    Extends: EndlessWikipedia.WikipediaApplication,

    _init: function(props) {
        this.parent(props);
    },

    vfunc_startup: function() {
        this.parent();
        this._domain_wiki_view = new DomainWikiView.DomainWikiView(this);
        let app_filename = this.application_uri;
        let linked_articles_filename = this.linked_articles_uri;
        this._domain_wiki_presenter = new DomainWikiPresenter.DomainWikiPresenter(this._domain_wiki_model, this._domain_wiki_view, app_filename, linked_articles_filename);
    }
});
