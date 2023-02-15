#include <iostream>
#include <string>
#include <ctemplate/template.h>
int main()
{
    std::string html = "./test.html";
    std::string html_info = "cwx";
    //建立ctemplate参数目录结构
    ctemplate::TemplateDictionary root("test"); //unordered_map<string,string> test;
    //向结构中添加你要替换的数据，kv的
    root.SetValue("key", html_info); //test.insert({key, value});
    //获取被渲染对象
    ctemplate::Template *tpl = ctemplate::Template::GetTemplate(html,
    ctemplate::DO_NOT_STRIP); //DO_NOT_STRIP：保持html网页原貌
    //开始渲染，返回新的网页结果到out_html
    std::string out_html;
    tpl->Expand(&out_html, &root);
    std::cout << "渲染的带参html是:" << std::endl;
    std::cout << out_html << std::endl;
    return 0;
}
