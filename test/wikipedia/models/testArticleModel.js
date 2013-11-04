const ArticleModel = imports.wikipedia.models.article_model;

let mockJsonData = {
    title: 'Article Title',
    url: 'file:///',
    source: 'Mock data',
    categories: [
        'Category One',
        'Category Two'
    ]
};

function _assertCategoryListHasIds(categoryList, idList) {
    assertEquals(idList.length, categoryList.length);
    idList.forEach(function (id) {
        assertTrue(categoryList.some(function (actualId) {
            return actualId == id;
        }));
    });
}

function testNewModelFromJson() {
    let model = ArticleModel.newFromJson(mockJsonData);
    assertTrue(model instanceof ArticleModel.ArticleModel);
    assertEquals('Article Title', model.title);
    assertEquals('file:///', model.uri);
    _assertCategoryListHasIds(model.getCategories(),
        ['Category One', 'Category Two']);
}

function testNewWithProperties() {
    let model = new ArticleModel.ArticleModel({
        title: 'Article Title',
        uri: 'file:///'
    });
    assertEquals('Article Title', model.title);
    assertEquals('file:///', model.uri);
    assertEquals(0, model.getCategories().length);
}

function testSetAndGetCategories() {
    let model = new ArticleModel.ArticleModel();
    let expectedCategories = ['One', 'Two', 'Three'];
    model.setCategories(expectedCategories);
    _assertCategoryListHasIds(model.getCategories(), expectedCategories);
}

function testSetCategoriesWipesPreviousCategories() {
    let model = new ArticleModel.ArticleModel();
    let firstCategories = ['One', 'Two', 'Three'];
    model.setCategories(firstCategories);
    let expectedCategories = ['A', 'B', 'C', 'D'];
    model.setCategories(expectedCategories);
    _assertCategoryListHasIds(model.getCategories(), expectedCategories);
}

function testAddAndGetCategories() {
    let model = new ArticleModel.ArticleModel();
    model.addCategory('One');
    model.addCategory('Two');
    model.addCategory('Three');
    _assertCategoryListHasIds(model.getCategories(), ['One', 'Two', 'Three']);
}

function testHasCategories() {
    let model = new ArticleModel.ArticleModel();
    let expectedCategories = ['One', 'Two', 'Three'];
    model.setCategories(expectedCategories);
    expectedCategories.forEach(function (id) {
        assertTrue(model.hasCategory(id));
    });
}
