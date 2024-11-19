#ifndef RB_TREE_H
#define RB_TREE_H

#include <cstdlib>

#include <fstream>
#include <string>
#include <vector>
#include <memory>

#include "log.h"

namespace RB
{

namespace detail
{

namespace html_colors
{
	static constexpr const char *blue_gray_ = "#48565D";
    static constexpr const char *coral_pink_ = "#F08080";
    static constexpr const char *navy_blue_ = "#0B0042";
    static constexpr const char *black_ = "#1B1B1C";
    static constexpr const char *red_ = "#820007";
};

};

template <typename KeyT> class Tree
{
  private:
	/* -----~ Usings ~----- */
	struct Node;
	using NodePtr = std::shared_ptr<Node>;

	/* -----~ Node ~----- */
    struct Node
    {
        KeyT value;

        NodePtr right{};
        NodePtr left{};
        NodePtr parent{};

        bool is_red = true;

        Node(const KeyT &_value)
            : value(_value)
        {}

        Node(const KeyT &_value, NodePtr _parent, bool _is_red = true)
            : value(_value)
            , parent(_parent)
            , is_red(_is_red)
        {}

        NodePtr get_grandp() const
        {
            if (parent != nullptr)
                return parent->parent;

            return nullptr;
        }

        NodePtr get_unc() const
        {
            NodePtr grandparent = get_grandp();

            if (grandparent == nullptr)
                return nullptr;

            return grandparent->right == parent ? grandparent->left
                                                : grandparent->right;
        }

        bool is_left_child() const { return parent->left.get() == this; }

        bool is_right_child() const { return !is_left_child(); }

        bool is_triangle() const
        {
            return (is_left_child() && parent->is_right_child()) ||
                   (is_right_child() && parent->is_left_child());
        }

        NodePtr get_bro() const
        {
            return is_left_child() ? parent->right : parent->left;
        }
    };

  	/* -----~ members ~----- */
    NodePtr root_ = nullptr;

  	/* -----~ private member-functions ~----- */
    void rotate_left(NodePtr node)
    {
        NodePtr pivot = node->right;

        pivot->parent = node->parent;

        if (node->parent != nullptr)
        {
            if (node->parent->left == node)
                node->parent->left = pivot;
            else
                node->parent->right = pivot;
        }
        else
            root_ = pivot;

        node->right = pivot->left;
        if (pivot->left != nullptr)
            pivot->left->parent = node;

        node->parent = std::move(pivot);
        node->parent->left = std::move(node);
    }

    void rotate_right(NodePtr node)
    {
        NodePtr pivot = node->left;

        pivot->parent = node->parent;
        if (node->parent != nullptr)
        {
            if (node->parent->left == node)
                node->parent->left = pivot;
            else
                node->parent->right = pivot;
        }
        else
            root_ = pivot;

        node->left = pivot->right;
        if (pivot->right != nullptr)
            pivot->right->parent = node;

        node->parent = std::move(pivot);
        node->parent->right = std::move(node);
    }

    void subtree_insert(NodePtr sub_root, const KeyT &value)
    {
        if (value > sub_root->value)
        {
            if (sub_root->right == nullptr)
            {
                NodePtr inserted_node = std::make_shared<Node>(value, sub_root);
                sub_root->right = inserted_node;

                fix_violation(inserted_node);

                return;
            }

            subtree_insert(sub_root->right, value);
        }
        else if (value < sub_root->value)
        {
            if (sub_root->left == nullptr)
            {
                NodePtr inserted_node = std::make_shared<Node>(value, sub_root);
                sub_root->left = inserted_node;

                fix_violation(inserted_node);

                return;
            }

            subtree_insert(sub_root->left, value);
        }

        return;
    }

    void handle_black_unc(NodePtr cur_node)
    {
        NodePtr parent = cur_node->parent;
        NodePtr grandparent = cur_node->get_grandp();

        if (cur_node->is_left_child() && parent->is_right_child())
        {
            rotate_right(parent);
            cur_node = cur_node->right;
        }
        else if (cur_node->is_right_child() && parent->is_left_child())
        {
            rotate_left(parent);
            cur_node = cur_node->left;
        }

        parent = cur_node->parent;
        grandparent = cur_node->get_grandp();

        if (cur_node->is_left_child())
            rotate_right(grandparent);
        else
            rotate_left(grandparent);

        paint_black(parent);
        paint_red(grandparent);
    }

    void fix_violation(NodePtr cur_node)
    {
        while (cur_node != root_ && cur_node->parent->is_red)
        {
            NodePtr parent = cur_node->parent;
            NodePtr uncle = cur_node->get_unc();

            if (uncle && uncle->is_red)
            {
                paint_black(parent);
                paint_black(uncle);

                NodePtr grandparent = cur_node->get_grandp();

                paint_red(grandparent);
                cur_node = grandparent;
            }
            else
            {
                handle_black_unc(cur_node);
                break;
            }
        }
        paint_black(root_);
    }

    void paint_black(NodePtr node) const
    {
        if (node != nullptr)
            node->is_red = false;
    }

    void paint_red(NodePtr node) const { node->is_red = true; }

    NodePtr create_based_on(const NodePtr reference, NodePtr parent)
    {
        if (reference == nullptr)
            return nullptr;

        LOG("Copying {}\n", reference->value);
        NodePtr created = std::make_shared<Node>(reference->value, parent, reference->is_red);

        created->left = create_based_on(reference->left, created);
        created->right = create_based_on(reference->right, created);

        return created;
    }

	/* -----~ graphviz dump ~----- */
    void dump_regular_nodes(NodePtr node, std::ostream &dump) const
    {
        if (node == nullptr)
            return;

        dump << "\t" << reinterpret_cast<size_t>(node.get())
             << "[shape = Mrecord, fillcolor = \""
             << (node->is_red ? detail::html_colors::red_ : detail::html_colors::black_)
			 << "\", label =  \"{" << node
             << " | val: " << node->value << " | {L: " << node->left.get()
             << " R: " << node->right.get() << "}}"
             << "\" ];\n";

        dump_regular_nodes(node->left, dump);

        dump_regular_nodes(node->right, dump);
    }

    void dump_connections(NodePtr node, std::ostream &dump) const
    {
        if (node == nullptr)
            return;

        if (node->left != nullptr)
        {
            dump << reinterpret_cast<size_t>(node.get()) << " -> "
                 << reinterpret_cast<size_t>(node->left.get()) << '\n';

            dump_connections(node->left, dump);
        }

        if (node->right != nullptr)
        {
            dump << reinterpret_cast<size_t>(node.get()) << " -> "
                 << reinterpret_cast<size_t>(node->right.get()) << '\n';

            dump_connections(node->right, dump);
        }
    }

  public:
    using iterator = NodePtr;

    Tree() = default;

    Tree(const Tree &other)
    {
        MSG("Copy constructor called\n");

        root_ = create_based_on(other.root_, nullptr);
    }

    Tree &operator=(const Tree &other)
    {
        MSG("Copy assignment called\n");

        if (this == &other)
            return *this;

        // free_data();

        root_ = create_based_on(other.root_, nullptr);

        return *this;
    }

    Tree(Tree &&other) noexcept
        : root_(std::move(other.root_))
        // , data_(std::move(other.data_))
    {
        MSG("Move constructor called\n");
        // other.data_.clear();
        other.root_ = nullptr;
    }

    Tree &operator=(Tree &&other) noexcept
    {
        MSG("Move assignment called\n");

        if (this == &other)
            return *this;

        // free_data();

        root_ = std::move(other.root_);
        // data_ = std::move(other.data_);

        // other.data_.clear();
        // other.root_ = nullptr;

        return *this;
    }

    void insert(const KeyT &value)
    {
        LOG("Inserting {}\n", value);

        if (root_ == nullptr)
        {
            root_ = std::make_shared<Node>(value);
            paint_black(root_);
        }
        else
            subtree_insert(root_, value);
    }

    void dump() const
    {
        std::string file_name = "tree_dump";
        std::ofstream dump(file_name + ".dot");

        dump << "digraph BinaryTree {\n"
                "bgcolor = \""
             << detail::html_colors::blue_gray_
             << "\";\n"
                "edge[minlen = 3, penwidth = 3; color = \"black\"];\n"
                "node[shape = \"rectangle\", style = \"rounded, filled\",\n"
                "\tfillcolor = \""
             << detail::html_colors::coral_pink_
             << "\",\n"
                "\tfontsize = 30,\n"
                "\theight = 3,\n"
                "\tpenwidth = 5, color = \"white\", fontcolor = \"white\"];\n";

        dump << "{rank = min;\n"
                "\tlist_manager [shape = Mrecord, fillcolor = \""
             << detail::html_colors::navy_blue_
             << "\", "
                "label = \"{ROOT: "
             << root_
             << "}\"];\n"
                "}\n";

        if (root_ != nullptr)
            dump_regular_nodes(root_, dump);

        dump_connections(root_, dump);

        dump << "}";
    }

    iterator lower_bound(const KeyT &key) const
    {
        iterator cur_node = root_;
        iterator answer{};

        if (!cur_node)
            return nullptr;

        while (true)
        {
            if (key < cur_node->value)
            {
                answer = cur_node;
                if (cur_node->left == nullptr)
                    return answer;
                cur_node = cur_node->left;
            }
            else if (key > cur_node->value)
            {
                if (cur_node->right == nullptr)
                    return answer;
                cur_node = cur_node->right;
            }
            else
                return cur_node;
        }

        return answer;
    }

    iterator upper_bound(const KeyT &key) const
    {
        iterator cur_node = root_;
        iterator answer = nullptr;

        if (!cur_node)
            return nullptr;

        while (true)
        {
            if (key < cur_node->value)
            {
                answer = cur_node;
                if (cur_node->left == nullptr)
                    return answer;
                cur_node = cur_node->left;
            }
            else if (key >= cur_node->value)
            {
                if (cur_node->right == nullptr)
                    return answer;
                cur_node = cur_node->right;
            }
        }

        return answer;
    }

    size_t distance(iterator fst, iterator snd) const
    {
        size_t distance = 0;

        if (!fst || (snd && (snd->value < fst->value)))
            return 0;

        NodePtr cur_node = fst;
        NodePtr last_visited = fst->left;

        while (cur_node != nullptr)
        {
            if (last_visited == cur_node->parent)
            {
                if (cur_node->left != nullptr)
                {
                    last_visited = cur_node;
                    cur_node = cur_node->left;
                    continue;
                }
                else
                    last_visited = nullptr;
            }

            if (last_visited == cur_node->left)
            {
                if (cur_node == snd)
                    break;

                ++distance;

                LOG("Counting {}\n", cur_node->value);

                if (cur_node->right != nullptr)
                {
                    last_visited = cur_node;
                    cur_node = cur_node->right;
                    continue;
                }
                else
                    last_visited = nullptr;
            }

            if (last_visited == cur_node->right)
            {
                last_visited = cur_node;
                cur_node = cur_node->parent;
            }
        }

        return distance;
    }

    ~Tree()
    {
        MSG("Destructor called\n");

        // free_data();
    }
};

};

#endif // RB_TREE_H
