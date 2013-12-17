const ArticleModel = imports.wikipedia.models.article_model;

describe("Wikipedia article model", function() {
    let mockJsonData = {
        title: 'Article Title',
        url: 'file:///',
        source: 'Mock data',
        categories: [
            'Category One',
            'Category Two'
        ]
    };

    describe("from JSON", function() {
        let model;

        beforeEach(function() {
            model = ArticleModel.newFromJson(mockJsonData);
        });

        it("has an article title", function() {
            expect(model.title).toEqual(mockJsonData.title);
        });

        it("has a uri", function() {
            expect(model.uri).toEqual(mockJsonData.url);
        });

        it("has a list of categories", function() {
            expect(model.getCategories()).toEqual(mockJsonData.categories);
        });
    });

    describe("from properties", function() {
        let model;
        beforeEach(function() {
            model = new ArticleModel.ArticleModel({
                title: 'Article Title',
                uri: 'file://'
            });
        });

        it("is an instance of an ArticleModel", function() {
            expect(model instanceof ArticleModel.ArticleModel).toBeTruthy();
        });

        it("has a title", function() {
            expect(model.title).toEqual('Article Title');
        });

        it("has a URI", function() {
            expect(model.uri).toEqual('file://');
        });

        it("has no categories", function() {
            expect(model.getCategories().length).toEqual(0);
        });
    });

    describe("setCategories method", function() {
        let model;

        beforeEach(function() {
            model = new ArticleModel.ArticleModel();
        });

        it("adds categories", function() {
            let expectedCategories = ['One', 'Two', 'Three'];
            model.setCategories(expectedCategories);
            expect(model.getCategories()).toEqual(expectedCategories);
        });

        it("replaces existing categories", function() {
            model.setCategories(['One', 'Two']);
            let expectedCategories = ['One', 'Two', 'Three'];
            model.setCategories(expectedCategories);
            expect(model.getCategories()).toEqual(expectedCategories);
        });
    });

    it("appends new categories on addCategory", function() {
        let model = new ArticleModel.ArticleModel();

        model.addCategory('One');
        model.addCategory('Two');
        model.addCategory('Three');
        expect(model.getCategories()).toEqual(['One', 'Two', 'Three']);
    });
    describe("hasCategory method", function() {
        let model;
        let expectedCategories = ['One', 'Two', 'Three'];
 
        beforeEach(function() {
            model = new ArticleModel.ArticleModel;
            model.setCategories(expectedCategories);
        });

        expectedCategories.forEach(function(category) {
            (function(categoryName) {
                 it("returns true for category named " + categoryName, function() {
                     expect(model.hasCategory(categoryName)).toBeTruthy();
                 });
             });
        });

        it("returns false for an unexpected category", function() {
            expect(model.hasCategory('unexpected')).toBeFalsy();
        });
    });
});
