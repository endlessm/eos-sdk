const DomainWikiModel = imports.wikipedia.models.domain_wiki_model;

let model;

let mockJsonData = {
    categories: [
        {
            category_name: 'Main Category',
            content_text: 'Lorem Ipsum',
            image_file: 'file:///image.jpg',
            image_thumb_uri: 'file:///image_thumb.jpg',
            is_main_category: true,
            subcategories: [
                'Category One',
                'Category Two'
            ]
        },
        {
            category_name: 'Category One',
            content_text: 'Lorem Ipsum',
            image_file: 'file:///image.jpg',
            image_thumb_uri: 'file:///image_thumb.jpg',
            is_main_category: false,
            subcategories: []
        },
        {
            category_name: 'Category Two',
            content_text: 'Lorem Ipsum',
            image_file: 'file:///image.jpg',
            image_thumb_uri: 'file:///image_thumb.jpg',
            is_main_category: false,
            subcategories: [
                'Category Three'
            ]
        },
        {
            category_name: 'Category Three',
            content_text: 'Lorem Ipsum',
            image_file: 'file:///image.jpg',
            image_thumb_uri: 'file:///image_thumb.jpg',
            is_main_category: false,
            subcategories: []
        },
    ],
    articles: [
        {
            title: 'Article One',
            url: 'file:///article1.html',
            categories: [
                'Category One'
            ]
        },
        {
            title: 'Article Two',
            url: 'file:///article2.html',
            categories: [
                'Category One',
                'Category Two'
            ]
        },
        {
            title: 'Article Three',
            url: 'file:///article3.html',
            categories: [
                'Category Two'
            ]
        }
    ]
};

function setUp() {
    model = new DomainWikiModel.DomainWikiModel();
}

function _assertArticleListContainsArticleTitles(articleList, titleList) {
    assertEquals(titleList.length, articleList.length);
    titleList.forEach(function (title) {
        assertTrue(articleList.some(function (articleModel) {
            return articleModel.title == title;
        }));
    });
}

function testGetArticlesReturnsAllArticles() {
    model.loadFromJson(mockJsonData);
    let articles = model.getArticles();
    _assertArticleListContainsArticleTitles(articles,
        [ 'Article One', 'Article Two', 'Article Three' ]);
}

function testGetArticlesForCategoryWithArticles() {
    model.loadFromJson(mockJsonData);
    let articles = model.getArticlesForCategory('Category One');
    _assertArticleListContainsArticleTitles(articles,
        [ 'Article One', 'Article Two' ]);
}

function testGetArticlesForCategoryWithoutArticles() {
    model.loadFromJson(mockJsonData);
    assertEquals(0, model.getArticlesForCategory('Main Category').length);
}

function testGetArticlesForCategoryWithNonexistentId() {
    assertEquals(0, model.getArticlesForCategory('Nonexistent').length);
}

function testCategoryHasArticlesReturnsTrue() {
    model.loadFromJson(mockJsonData);
    assertTrue(model._getCategoryHasArticles('Category Two'));
}

function testCategoryHasArticlesReturnsFalse() {
    model.loadFromJson(mockJsonData);
    assertFalse(model._getCategoryHasArticles('Category Three'));
}

function testCategoryHasArticlesWithNonexistentId() {
    assertFalse(model._getCategoryHasArticles('Nonexistent'));
}

function testGetCategory() {
    model.loadFromJson(mockJsonData);
    let category = model.getCategory('Category One');
    assertEquals('CategoryModel', category.__name__);
    assertEquals('Category One', category.title);
}

function testGetNonexistentCategory() {
    assertUndefined(model.getCategory('Nonexistent'));
}

function testGetMainCategory() {
    model.loadFromJson(mockJsonData);
    let category = model.getMainCategory();
    assertTrue(category.__name__ == 'CategoryModel');
    assertEquals('Main Category', category.title);
}

function testGetUnsetMainCategory() {
    assertNull(model.getMainCategory());
}
