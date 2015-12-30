# 关于在_storyboard_中使用静态TableViewCell的可变高度的使用技巧
> `起因：`在`storyboard`使用之前，创建`tableView`视图，通常是采用代码控制数据源，通过数据源数据的长度来决定`tableView`中`cell`的个数，而这样做的好处还有就是可以通过将已经在界面中展示的cell放入缓存池中，以供后续数据展示再从缓存池中取出进行复用。这样做的好处是为了防止数据过大时创建大量的cell浪费不必要的空间。显而易见，`tableView`就是为了进行批量数据的展示。   
> 然而，在现实开发中，会出现一种需求：展示的数据长度是确定的，而且需要向`tableView`一样实现视图上下滚动，单个`cell`视图可点击响应。这种情况下，继续使用原来的代码数据源控制一样可以实现，这时的数据源个数是确定的。即便如此，你也需要考虑处理`cell`滚动时发生的重用所带来的影响。而且当数据源确定时，你的产品经理绝对不甘于像平常一样每条数据都展示为同一个尺寸内容。而是会尽可能地在每个`cell`中加入丰富的控件，时间相应来是有限的`tableView`实现最为复杂的用户交互事件。此时，你会在自己的数据源拼命地书写if语句来保证视图的正确显示，又要避免发生未知的`cell`重用问题。  
> 这样做是可行的，但是在主要时间将大量花费在处理有限个数`cell`上的逻辑处理，而并非我们所要实现的主要业务。  
> 所以，在这种情况下，我们会考虑把这些重复而且无用的逻辑处理交给苹果自己搞定（当然，主要目的是为了偷懒^_^）  
> `static cell`，这是苹果为我们提供的解决方案。目前仅限于在`storyboard`中使用。  
> 
> `static cell`的用法非常简单，这里不作为重点赘述，请大家自行学习。  
> 现在给大家这样一个场景，在你用`static cell`搭建好界面，然后轻松加愉快的拖拖控件，完成相关业务功能。  
> 上面老板开始验收项目，对产品说你的界面不够详细，再改改。  
> 产品开始提需求，说我们的cell内容太少，需要在点击时展开详情。  
> 而恰好你用静态单元cell偷了个懒，现在要为了一个cell重新用动态cell写一遍？  
> 别闹了，明天还要和UI妹子去爬香山呢，绝壁不当加班狗。  
> 所以，我们开始探索静态cell的高度改变之旅。
> ***
> 方案一：
> 既然我们可以将静态cell当做一个控件拖入控制器做属性。何不在代码中修改作为属性的静态cell的高度呢？
```objc   
		self.fuJianCell.height = 100;		
    	[self.fuJianCell layoutIfNeeded];
		[self.tableView reloadData];
```
然并卵，这些方法对非cell的控件来说刷新高度足矣，然而`tableView`的高度始终是由代理来控制的，所以这样修改是没用的。  
既然这样，我们就从高度代理入手。  

***

方案二：
通过代理入手，将每个cell的在storyBoard的设置高度用代码写到一个数组中，并通过代理返回。
```objc
- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
	// 变高行数
	NSInteger mutableHeightRow = 1;
	// 如果不展开
	if(!isOpen){
		self.heights[mutableHeightRow] = @(原始高度);
		return [self.heights[indexPath.row] integerValue];
	} else {
		self.heights[mutableHeightRow] = @(展开高度);
		return [self.heights[indexPath.row] integerValue];
	}
}
```
问题解决，但是违背了我们的初衷，说好的交给苹果自己算呢，为毛要手动将高度存在数组中？

***

方案三：
交给tableView的父类方法返回我们在storyboard中预设的高度，然后单独处理变高的cell高度
```objc
- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
	// 变高行数
	NSInteger mutableHeightRow = 1;
    if (indexPath.row == mutableHeightRow) {
        if (!isOpen) {
        	// 假设改行原来高度为200
        	return 80;
        } else {
            return [super tableView:tableView heightForRowAtIndexPath:indexPath];
        }
    } else {
        /** 返回静态单元格故事板中的高度 */
        return [super tableView:tableView heightForRowAtIndexPath:indexPath];
    }
}
```
完美~
***

```objc
// 注意别忘了,在修改展开状态的时候刷新表格
	isOpen = YES;
	[self.tableView reloadData];
```

其实，这样处理静态cell的方式不光可以处理可变高度，还可以用在数据源的section个数或者每个section的row的个数上，再或者指定让某个位置的cell使用动态cell其他使用静态cell，让我们的静态cell比动态cell更好用。其方法都是通过`super`关键字调用父类方法返回storyboard中存储的的相关数据。

至于原理，暂时还不清楚，推测使用了运行时动态创建一个支持静态cell的tableView的子类，我们实际使用的是这个新的tableView类的实例，或者使用`类簇`，利用私有子类进行了相关的静态cell的逻辑处理。

总之，这让我们省去了很多处理逻辑的代码。

参考文章：[UITableView的：在处理混合cell表视图的静态和动态的cell小区选择](http://codego.net/513617/)
 