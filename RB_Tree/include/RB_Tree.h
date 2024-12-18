#ifndef RB_TREE_H
#define RB_TREE_H

#include <cstdlib>

#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <stack>

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
}; // namespace html_colors

}; // namespace detail

template <typename KeyT, typename Compare = std::less<KeyT>>
class Tree
{
  private:
    /* -----~ Usings ~----- */
    struct Node;
    using NodePtr = std::unique_ptr<Node>;

    /* -----~ Node ~----- */
    struct Node
    {
        KeyT value;

        Node* right{};
        Node* left{};
        Node* parent{};

        bool is_red = true;

        Node(const KeyT &_value)
            : value(_value)
        {}

        Node(const KeyT &_value, Node* _parent, bool _is_red = true)
            : value(_value)
            , parent(_parent)
            , is_red(_is_red)
        {}

        Node* get_grandp() const
        {
			return parent ? parent->parent : nullptr;
        }

        Node* get_unc() const
        {
            Node* grandparent = get_grandp();

            if (grandparent == nullptr) return nullptr;

            return 	  grandparent->right == parent
					? grandparent->left
                    : grandparent->right;
        }

        bool is_left_child() const
        {
            return parent && parent->left == this;
        }

        bool is_right_child() const
        {
            return parent && parent->right == this;
        }

        bool is_triangle() const
        {
            return (is_left_child() && parent->is_right_child()) ||
                   (is_right_child() && parent->is_left_child());
        }
    };

	/* -----~ members ~----- */
    Node* root_ = nullptr;
	std::vector<NodePtr> data_;
	Compare cmp_;

    /* -----~ Iterator ~----- */

    static Node* sub_begin(Node* node)
    {
        while (node && node->left)
            node = node->left;
        return node;
    }

    static Node* sub_end(Node* node)
    {
        while (node && node->right)
            node = node->right;
        return node;
    }

    static Node* next(Node* node)
    {
        if (node->right)
            return sub_begin(node->right);

        Node* parent = node->parent;

        while (parent && node == parent->right)
        {
            node = parent;
            parent = parent->parent;
        }

        return parent;
    }

    static Node* prev(Node* node)
    {
        if (node->left)
            return sub_end(node->left);

        Node* parent = node->parent;
        while (parent && node == parent->left)
        {
            node = parent;
            parent = parent->parent;
        }

        return parent;
    }

  public:
    class iterator
    {
      public:
        using value_type = KeyT;
        using difference_type = std::ptrdiff_t;
        using reference = KeyT&;
        using iterator_category = std::bidirectional_iterator_tag;
        using pointer = Node*;

	  private:
        pointer node_;

      public:

        iterator(pointer node = nullptr)
            : node_(node)
        {}

        iterator(const iterator &other)
            : node_(other.node_)
        {}

        iterator &operator=(const iterator &other)
        {
            node_ = other.node_;
            return *this;
        }

        KeyT &operator*() const { return node_->value; }

        iterator &operator++()
        {
            if (node_)
                LOG("incrementing iterator of value {}\n", node_->value);
            else
                MSG("incrementing null iterator\n");

            node_ = Tree::next(node_);
            return *this;
        }

        iterator operator++(KeyT)
        {
            if (node_)
                LOG("postincrementing iterator of value {}\n", node_->value);
            else
                MSG("postincrementing null iterator\n");

            iterator tmp(*this);
            node_ = Tree::next(node_);
            return tmp;
        }

        iterator &operator--()
        {
            if (node_)
                LOG("decrementing iterator of value {}\n", node_->value);
            else
                MSG("decrementing null iterator\n");

            node_ = Tree::prev(node_);
            return *this;
        }

        iterator operator--(KeyT)
        {
            if (node_)
                LOG("postdecrementing iterator of value {}\n", node_->value);
            else
                MSG("postdecrementing null iterator\n");

            iterator tmp(*this);
            node_ = Tree::prev(node_);
            return tmp;
        }

        bool operator==(const iterator &other) const
        {
            MSG("operator== called\n");

            return node_ == other.node_;
        }

        bool operator!=(const iterator &other) const
        {
			MSG("operator!= called\n");
            return !(*this == other);
        }
    };

    iterator beign() { return iterator(sub_begin(root_)); }
    iterator end() { return iterator(nullptr); }

  private:
    /* -----~ private member-functions ~----- */
    void rotate_left(Node* node)
    {
        Node* pivot = node->right;

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

        node->parent = pivot;
        node->parent->left = node;
    }

    void rotate_right(Node* node)
    {
        Node* pivot = node->left;

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

        node->parent = pivot;
        node->parent->right = node;
    }

    void subtree_insert(Node* cur_node, const KeyT &value)
    {
		while (cur_node)
		{
			if (cmp_(cur_node->value, value))
			{
				if (cur_node->right == nullptr)
				{
					auto inserted_node = std::make_unique<Node>(value, cur_node);

					Node* inserted_raw = inserted_node.get();

					cur_node->right = inserted_raw;

					data_.push_back(std::move(inserted_node));

					fix_violation(inserted_raw);

					return;
				}

				cur_node = cur_node->right;
				continue;
			}
			else if (cmp_(value, cur_node->value))
			{
				if (cur_node->left == nullptr)
				{
					auto inserted_node = std::make_unique<Node>(value, cur_node);

					Node* inserted_raw = inserted_node.get();

					data_.push_back(std::move(inserted_node));

					cur_node->left = inserted_raw;

					fix_violation(inserted_raw);

					return;
				}

				cur_node = cur_node->left;
				continue;
			}

			return;
		}
    }

    void handle_black_unc(Node* cur_node)
    {
        Node* parent = cur_node->parent;
        Node* grandparent = cur_node->get_grandp();

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

    void fix_violation(Node* cur_node)
    {
        while (cur_node != root_ && cur_node->parent->is_red)
        {
            Node* parent = cur_node->parent;
            Node* uncle = cur_node->get_unc();

            if (uncle && uncle->is_red)
            {
                paint_black(parent);
                paint_black(uncle);

                Node* grandparent = cur_node->get_grandp();

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

    void paint_black(Node* node) const
    {
        if (node != nullptr)
            node->is_red = false;
    }

    void paint_red(Node* node) const { node->is_red = true; }

    /* -----~ graphviz dump ~----- */
    void dump_regular_nodes(Node* node, std::ostream &dump) const
    {
        if (node == nullptr)
            return;

        dump << "\t" << reinterpret_cast<size_t>(node)
             << "[shape = Mrecord, fillcolor = \""
             << (node->is_red ? detail::html_colors::red_
                              : detail::html_colors::black_)
             << "\", label =  \"{" << node << " | val: " << node->value
             << " | {L: " << node->left << " R: " << node->right
             << "}}"
             << "\" ];\n";

        dump_regular_nodes(node->left, dump);

        dump_regular_nodes(node->right, dump);
    }

    void dump_connections(Node* node, std::ostream &dump) const
    {
        if (node == nullptr)
            return;

        if (node->left != nullptr)
        {
            dump << reinterpret_cast<size_t>(node) << " -> "
                 << reinterpret_cast<size_t>(node->left) << '\n';

            dump_connections(node->left, dump);
        }

        if (node->right != nullptr)
        {
            dump << reinterpret_cast<size_t>(node) << " -> "
                 << reinterpret_cast<size_t>(node->right) << '\n';

            dump_connections(node->right, dump);
        }
    }


  public:
	/* -----~ public member-functions ~----- */
	Tree() = default;

	Tree(const Tree &other)
	{
		MSG("Copy constructor called\n");

		if (!other.root_) return;

		struct NodeContext
		{
			Node* original;
			Node* copy;
		};

		std::stack<NodeContext> stack;

		auto root = std::make_unique<Node>(other.root_->value, nullptr, other.root_->is_red);
		root_ = root.get();
		data_.push_back(std::move(root));

		stack.push({other.root_, root_});

		while (!stack.empty())
		{
			NodeContext ctxt = stack.top();
			stack.pop();

			if (ctxt.original->left)
			{
				auto left =
					std::make_unique<Node>(	ctxt.original->left->value,
											ctxt.copy,
											ctxt.original->left->is_red);
				ctxt.copy->left = left.get();
				data_.push_back(std::move(left));

				stack.push({ctxt.original->left, ctxt.copy->left});
			}

			if (ctxt.original->right)
			{
				auto right =
					std::make_unique<Node>(	ctxt.original->right->value,
											ctxt.copy,
											ctxt.original->right->is_red);
				ctxt.copy->right = right.get();
				data_.push_back(std::move(right));

				stack.push({ctxt.original->right, ctxt.copy->right});
			}
		}
	}

	Tree(Tree&& other) noexcept: Tree() { swap(other); }

	Tree &operator=(const Tree& other)
	{
		MSG("Copy assignment called\n");

		if (this == &other)
			return *this;

		Tree copied_tree(other);

		swap(copied_tree);

		return *this;
	}

	Tree& operator=(Tree&& other) noexcept
	{
		if (this == &other)
			return *this;

		Tree moved_tree(std::move(other));

		swap(moved_tree);

		return *this;
	}

    void insert(const KeyT &value)
    {
        LOG("Inserting {}\n", value);

        if (root_ == nullptr)
        {
			auto root = std::make_unique<Node>(value);
            root_ = root.get();
			data_.push_back(std::move(root));

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
        Node* cur_node = root_;
        Node* answer{};

        if (!cur_node)
            return iterator{};

        while (cur_node)
        {
            if (cmp_(key, cur_node->value))
            {
                answer = cur_node;
                if (cur_node->left == nullptr)
                    return iterator(answer);
                cur_node = cur_node->left;
            }
            else if (cmp_(cur_node->value, key))
            {
                if (cur_node->right == nullptr)
                    return iterator(answer);
                cur_node = cur_node->right;
            }
            else
                return iterator(cur_node);
        }

        return iterator(answer);
    }

    iterator upper_bound(const KeyT &key) const
    {
        Node* cur_node = root_;
        Node* answer = nullptr;

        if (!cur_node)
            return iterator{};

        while (cur_node)
        {
            if (cmp_(key, cur_node->value))
            {
                answer = cur_node;
                if (cur_node->left == nullptr)
                    return iterator(answer);
                cur_node = cur_node->left;
            }
            else
            {
                if (cur_node->right == nullptr)
                    return iterator(answer);
                cur_node = cur_node->right;
            }
        }

        return iterator(answer);
    }

	void swap(Tree& other) noexcept
	{
		using std::swap;

		swap(root_, other.root_);
		swap(data_, other.data_);
	}
};

}; // namespace RB

#endif // RB_TREE_H
