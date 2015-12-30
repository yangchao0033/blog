<div class="post">
		<h1 class="postTitle">
			<a id="cb_post_title_url" class="postTitle2">关于block使用的5点注意事项【转】</a>
			</br>
			<h5>2015年12月21日 下午1:50</h5>
			</br>
		</h1>
		<h4 class="title2">
			<a id="cb_post_title_url" class="postTitle3" href="http://www.cnblogs.com/biosli/p/block_usage.html">原文链接(http://www.cnblogs.com/biosli/p/block_usage.html)</a>
		</h4>
		<div class="clear"></div>
		<div class="postBody">
			<div id="cnblogs_post_body"><div id="main-content" class="wiki-content">
<p>1、在使用block前需要对block指针做判空处理。</p>
<p>不判空直接使用，一旦指针为空直接产生崩溃。</p>
</div>
<div class="wiki-content">
<div class="cnblogs_code"><div class="cnblogs_code_toolbar"><span class="cnblogs_code_copy"><a href="javascript:void(0);" onclick="copyCnblogsCode(this)" title="复制代码"><img src="http://common.cnblogs.com/images/copycode.gif" alt="复制代码"></a></span></div>
<pre><span style="color: #0000ff;">if</span> (!<span style="color: #000000;">self.isOnlyNet) {
    </span><span style="color: #0000ff;">if</span> (succBlock == NULL) { <span style="color: #008000;">//</span><span style="color: #008000;">后面使用block之前要先做判空处理</span>
        <span style="color: #0000ff;">return</span><span style="color: #000000;">;
    }
    </span><span style="color: #0000ff;">id</span> data =<span style="color: #000000;"> [NSKeyedUnarchiver unarchiveObjectWithFile:[self favoriteFile]];
    </span><span style="color: #0000ff;">if</span> ([data isKindOfClass:[NSMutableArray <span style="color: #0000ff;">class</span><span style="color: #000000;">]]) {
        succBlock(data,YES);
    }</span><span style="color: #0000ff;">else</span><span style="color: #000000;">{
        succBlock(nil,YES);
    }
}</span></pre>
<div class="cnblogs_code_toolbar"><span class="cnblogs_code_copy"><a href="javascript:void(0);" onclick="copyCnblogsCode(this)" title="复制代码"><img src="http://common.cnblogs.com/images/copycode.gif" alt="复制代码"></a></span></div></div>
<p>&nbsp;</p>
</div>
<div class="wiki-content">2、在MRC的编译环境下，block如果作为成员参数要copy一下将栈上的block拷贝到堆上（示例见下，<a class="external-link" href="http://www.cnblogs.com/biosli/archive/2013/05/29/iOS_Objective-C_Block.html" rel="nofollow">原因参考</a>）
<p>3、在block使用之后要对，block指针做赋空值处理，如果是MRC的编译环境下，要先release掉block对象。</p>
<p>block作为类对象的成员变量，使用block的人有可能用类对象参与block中的运算而产生循环引用。</p>
<p>将block赋值为空，是解掉循环引用的重要方法。（不能只在dealloc里面做赋空值操作，这样已经产生的循环引用不会被破坏掉）</p>
<div class="cnblogs_code"><div class="cnblogs_code_toolbar"><span class="cnblogs_code_copy"><a href="javascript:void(0);" onclick="copyCnblogsCode(this)" title="复制代码"><img src="http://common.cnblogs.com/images/copycode.gif" alt="复制代码"></a></span></div>
<pre>typedef <span style="color: #0000ff;">void</span>(^SuccBlock)(<span style="color: #0000ff;">id</span><span style="color: #000000;"> data);
</span><span style="color: #0000ff;">@interface</span><span style="color: #000000;"> NetworkClass {
    SuccessBlock _sucBlock;
}
@property (nonatomic,assign)BOOL propertyUseInCallBack;
</span>- (<span style="color: #0000ff;">void</span><span style="color: #000000;">) requestWithSucBlock: (SuccessBlock) callbackBlock;
</span><span style="color: #0000ff;">@end</span>
 
<span style="color: #0000ff;">@implementation</span><span style="color: #000000;"> NetworkClass
</span>- (<span style="color: #0000ff;">void</span><span style="color: #000000;">) requestWithSucBlock: (SuccessBlock) callbackBlock {
    _sucBlock </span>= callbackBlock;<span style="color: #008000;">//</span><span style="color: #008000;">MRC下：_sucBlock = [callbackBlock copy]; 不copy block会在栈上被回收。</span>
<span style="color: #000000;">}
 
</span>- (<span style="color: #0000ff;">void</span>) netwrokDataBack: (<span style="color: #0000ff;">id</span><span style="color: #000000;">) data {
    </span><span style="color: #0000ff;">if</span> (data != nil &amp;&amp; _sucBlock !=<span style="color: #000000;"> NULL) {
        _sucBlock(data);
    }
    </span><span style="color: #008000;">//</span><span style="color: #008000;">MRC下：要先将[_sucBlock release];（之前copy过）</span>
    _sucBlock = nil; <span style="color: #008000;">//</span><span style="color: #008000;">Importent: 在使用之后将Block赋空值，解引用 !!!</span>
<span style="color: #000000;">}
</span><span style="color: #0000ff;">@end</span>
 
<span style="color: #008000;">//</span><span style="color: #008000;">=======================以下是使用方===========================</span>
<span style="color: #0000ff;">@implementation</span><span style="color: #000000;"> UserCode
</span>- (<span style="color: #0000ff;">void</span><span style="color: #000000;">) temporaryNetworkCall
{
    NetworkClass </span>*netObj =<span style="color: #000000;"> [[NetworkClass alloc] init];
    netObj.propertyUseInCallBack </span>=<span style="color: #000000;"> NO;
    [netObj requestWithSucBlock: </span>^(<span style="color: #0000ff;">id</span><span style="color: #000000;"> data) {
        </span><span style="color: #008000;">//</span><span style="color: #008000;">由于block里面引用netObj的指针所以这里产生了循环引用，且由于这个block是作为参数传入对象的，编译器不会报错。
        </span><span style="color: #008000;">//</span><span style="color: #008000;">因此，NetworkClass使用完block之后一定要将作为成员变量的block赋空值。</span>
        <span style="color: #0000ff;">if</span> (netObj.propertyUseInCallBack ==<span style="color: #000000;"> YES) {
            </span><span style="color: #008000;">//</span><span style="color: #008000;">Do Something...</span>
<span style="color: #000000;">        }
    }];
}
</span><span style="color: #0000ff;">@end</span></pre>
<div class="cnblogs_code_toolbar"><span class="cnblogs_code_copy"><a href="javascript:void(0);" onclick="copyCnblogsCode(this)" title="复制代码"><img src="http://common.cnblogs.com/images/copycode.gif" alt="复制代码"></a></span></div></div>
<p>&nbsp;</p>
<p>还有一种改法，在block接口设计时，将可能需要的变量作为形参传到block中，从设计上解决循环引用的问题。</p>
<p>如果上面Network类设计成这个样子：</p>
<div class="code panel pdl" style="border-width: 1px;">
<div class="codeHeader panelHeader pdl" style="border-bottom-width: 1px;">
<div class="cnblogs_code"><div class="cnblogs_code_toolbar"><span class="cnblogs_code_copy"><a href="javascript:void(0);" onclick="copyCnblogsCode(this)" title="复制代码"><img src="http://common.cnblogs.com/images/copycode.gif" alt="复制代码"></a></span></div>
<pre><span style="color: #0000ff;">@class</span><span style="color: #000000;"> NetowrkClass;
typedef </span><span style="color: #0000ff;">void</span>(^SuccBlock)(NetworkClass *aNetworkObj, <span style="color: #0000ff;">id</span><span style="color: #000000;"> data);
</span><span style="color: #0000ff;">@interface</span><span style="color: #000000;"> NetworkClass
</span><span style="color: #008000;">//</span><span style="color: #008000;">...</span>
<span style="color: #0000ff;">@end</span>
<span style="color: #0000ff;">@implementation</span><span style="color: #000000;"> NetworkClass
</span><span style="color: #0000ff;">@end</span>
 
<span style="color: #0000ff;">@implementation</span><span style="color: #000000;"> UserCode
</span>- (<span style="color: #0000ff;">void</span><span style="color: #000000;">) temporaryNetworkCall
{
    NetworkClass </span>*netObj =<span style="color: #000000;"> [[NetworkClass alloc] init];
    netObj.propertyUseInCallBack </span>=<span style="color: #000000;"> NO;
    [netObj requestWithSucBlock: </span>^(NetworkClass *aNetworkObj, <span style="color: #0000ff;">id</span><span style="color: #000000;"> data) {
        </span><span style="color: #008000;">//</span><span style="color: #008000;">这里参数中已经有netObj的对象了，使用者不用再从block外引用指针了。</span>
        <span style="color: #0000ff;">if</span> (aNetworkObj.propertyUseInCallBack ==<span style="color: #000000;"> YES) {
            </span><span style="color: #008000;">//</span><span style="color: #008000;">Do Something...</span>
<span style="color: #000000;">        }
    }];
}
</span><span style="color: #0000ff;">@end</span></pre>
<div class="cnblogs_code_toolbar"><span class="cnblogs_code_copy"><a href="javascript:void(0);" onclick="copyCnblogsCode(this)" title="复制代码"><img src="http://common.cnblogs.com/images/copycode.gif" alt="复制代码"></a></span></div></div>
<p>&nbsp;</p>
</div>
</div>
<p>4、使用方将self或成员变量加入block之前要先将self变为__weak</p>
<p>5、在多线程环境下（block中的weakSelf有可能被析构的情况下），需要先将self转为strong指针，避免在运行到某个关键步骤时self对象被析构。</p>
<p>第四、第五条合起来有个名词叫weak–strong dance，来自于<a class="external-link" style="text-decoration: none;" href="https://www.google.com.hk/url?sa=t&amp;rct=j&amp;q=&amp;esrc=s&amp;source=web&amp;cd=1&amp;ved=0CDkQFjAA&amp;url=%68%74%74%70%3a%2f%2f%64%65%76%65%6c%6f%70%65%72%2e%61%70%70%6c%65%2e%63%6f%6d%2f%76%69%64%65%6f%73%2f%77%77%64%63%2f%32%30%31%31%2f&amp;ei=mDl_UcKaEtDZkgXG14GoDg&amp;usg=AFQjCNENM0Mbw7vrvpPYNzjg2R9u6IUiEA&amp;sig2=wwwajZBQ62fO7EsU4RvSOg&amp;bvm=bv.45645796,d.dGI&amp;cad=rjt" rel="nofollow">2011 WWDC Session #322 (Objective-C Advancements in Depth）</a></p>
<p>以下代码来自AFNetworking，堪称使用weak–strong dance的经典。</p>
<div class="code panel pdl" style="border-width: 1px;">
<div class="codeHeader panelHeader pdl" style="border-bottom-width: 1px;">
<div class="cnblogs_code"><div class="cnblogs_code_toolbar"><span class="cnblogs_code_copy"><a href="javascript:void(0);" onclick="copyCnblogsCode(this)" title="复制代码"><img src="http://common.cnblogs.com/images/copycode.gif" alt="复制代码"></a></span></div>
<pre>__weak __typeof(self)weakSelf =<span style="color: #000000;"> self;
AFNetworkReachabilityStatusBlock callback </span>= ^<span style="color: #000000;">(AFNetworkReachabilityStatus status) {
    __strong __typeof(weakSelf)strongSelf </span>=<span style="color: #000000;"> weakSelf;
    strongSelf.networkReachabilityStatus </span>=<span style="color: #000000;"> status;
    </span><span style="color: #0000ff;">if</span><span style="color: #000000;"> (strongSelf.networkReachabilityStatusBlock) {
        strongSelf.networkReachabilityStatusBlock(status);
    }
};</span></pre>
<div class="cnblogs_code_toolbar"><span class="cnblogs_code_copy"><a href="javascript:void(0);" onclick="copyCnblogsCode(this)" title="复制代码"><img src="http://common.cnblogs.com/images/copycode.gif" alt="复制代码"></a></span></div></div>
</div>
<div class="codeHeader panelHeader pdl" style="border-bottom-width: 1px;">Review一下上面这段代码，里面玄机不少。</div>
</div>
<p>第一行：__weak __typeof(self)weakSelf = self;</p>
<p>如之前第四条所说，为防止callback内部对self强引用，weak一下。</p>
<p>其中用到了__typeof(self)，这里涉及几个知识点：</p>
<p>a. __typeof、__typeof__、typeof的区别</p>
<p>恩~~他们没有区别，但是这牵扯一段往事，在早期C语言中没有typeof这个关键字，__typeof、__typeof__是在C语言的扩展关键字的时候出现的。</p>
<p>typeof是现代GNU C++的关键字，从Objective-C的根源说，他其实来自于C语言，所以AFNetworking使用了继承自C的关键字。</p>
<p>b.对于老的LLVM编译器上面这句话会编译报错，所以在很早的ARC使用者中流行__typeof(&amp;*self)这种写法，<a class="external-link" href="http://stackoverflow.com/questions/10892361/generic-typeof-for-weak-self-references/" rel="nofollow">原因如下</a></p>
<p>大致说法是老LLVM编译器会将__typeof转义为 XXX类名 *const __strong的__strong和前面的__weak关键字对指针的修饰又冲突了，所以加上&amp;*对指针的修饰。</p>
<p>第三行：__strong __typeof(weakSelf)strongSelf = weakSelf;</p>
<p>按照之前第五条的说法给转回strong了，这里__typeof()里面写的是weakSelf，里面写self也没有问题，因为typeof是编译时确定变量类型，所以这里写self 不会被循环引用。</p>
<p>第四、五、六行，如果不转成strongSelf而使用weakSelf，后面几句话中，有可能在第四句执行之后self的对象可能被析构掉，然后后面的StausBlock没有执行，导致逻辑错误。</p>
<p>最后第五行，使用前对block判空。</p>
<p>&nbsp;</p>
<p>写在最后，阅读好的开源库源码是提高个人水平的一个很好途径，看见不懂的地方去查去摸索会得到更多。</p>
<p>&nbsp;</p>
<p>参考：</p>
<p><a class="external-link" href="http://blog.csdn.net/fhbystudy/article/details/17350951" rel="nofollow">http://blog.csdn.net/fhbystudy/article/details/17350951</a></p>
<p><a class="external-link" href="http://stackoverflow.com/questions/10892361/generic-typeof-for-weak-self-references" rel="nofollow">http://stackoverflow.com/questions/10892361/generic-typeof-for-weak-self-references</a></p>
<p><a class="external-link" href="http://stackoverflow.com/questions/14877415/difference-between-typeof-typeof-and-typeof-objective-c" rel="nofollow">http://stackoverflow.com/questions/14877415/difference-between-typeof-typeof-and-typeof-objective-c</a></p>
<p><a class="external-link" href="http://rocry.com/2012/12/18/objective-c-type-of/" rel="nofollow">http://rocry.com/2012/12/18/objective-c-type-of/</a></p>
<p><a class="external-link" href="http://fuckingblocksyntax.com/" rel="nofollow">http://fuckingblocksyntax.com/</a> 这是个工具网站，我每次写block的时候都会用到。</p>
</div>
<div id="xunlei_com_thunder_helper_plugin_d462f475-c18e-46be-bd10-327458d045bd">&nbsp;</div></div><div id="MySignature"></div>
<div class="clear"></div>