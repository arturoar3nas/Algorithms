#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <ostream>
#include <functional>

class appContext;
class MonoappA;
class MonoappB;
class StereoappA;
class StereoappB;


class IIRFilter
{
  public:
    int value;
};

class GaussianBlurClass
{
  public:
    int value;
};

class ImageRegions
{
  public:
    int value;
};

class app
{
  public:
    std::shared_ptr<IIRFilter> iir_filter;
    std::shared_ptr<GaussianBlurClass> gaussian_filter;
    std::shared_ptr<ImageRegions> regions;
    std::shared_ptr<appContext> context;

    ~app()
    {
      std::cout << "~app\n";
    }

    friend std::ostream& operator<<(std::ostream& os, const app* py)
    {
      return os << "iir filter: " << py->iir_filter->value << std::endl
                << "gaussian filter: " << py->gaussian_filter->value << std::endl
                << "regions: " << py->regions->value << std::endl;
    }

    friend std::ostream& operator<<(std::ostream& os, const std::unique_ptr<app>& py)
    {
      return os << "iir filter: " << py->iir_filter->value << std::endl
                << "gaussian filter: " << py->gaussian_filter->value << std::endl
                << "regions: " << py->regions->value << std::endl;
    }
};

/* Builder is responsible for constructing the smaller parts */
class appBuilder
{
  public:
    virtual std::shared_ptr<IIRFilter>  getIIRFilter() = 0;
    virtual std::shared_ptr<GaussianBlurClass> getGaussianFilter() = 0;
    virtual std::shared_ptr<ImageRegions> getRegions() = 0;
    virtual std::shared_ptr<appContext> getContext() = 0;
};

/* Director is responsible for the whole process */
class Director
{
  std::shared_ptr<appBuilder> builder;

  public:
    void setBuilder(std::shared_ptr<appBuilder> _builder)
    {
      builder = _builder;
    }

    std::unique_ptr<app> getapp()
    {
      app app;

      app.iir_filter = builder->getIIRFilter();
      app.gaussian_filter = builder->getGaussianFilter();
      app.regions = builder->getRegions();
      app.context = builder->getContext();

      return std::make_unique<app>(app);
    }
};

/* Concrete Builder for monoespectral app */
class MonoBuilder_A : public appBuilder
{
  public:
    std::shared_ptr<IIRFilter>  getIIRFilter()
    {
      auto iir = std::make_shared<IIRFilter>();
      iir->value = 1;
      return iir;
    }

    std::shared_ptr<GaussianBlurClass> getGaussianFilter()
    {
      auto gauss = std::make_shared<GaussianBlurClass>();
      gauss->value = 10;
      return gauss;
    }

    std::shared_ptr<ImageRegions> getRegions()
    {
      auto regions = std::make_shared<ImageRegions>();
      regions->value = 100;
      return regions;
    }

    std::shared_ptr<appContext> getContext()
    {
      return std::make_unique<appContext>(std::make_shared<MonoappA>());
    }
};

/* Concrete Builder for monoespectral app */
class MonoBuilder_B : public appBuilder
{
  public:
    std::shared_ptr<IIRFilter>  getIIRFilter()
    {
      auto iir = std::make_shared<IIRFilter>();
      iir->value = 3;
      return iir;
    }

    std::shared_ptr<GaussianBlurClass> getGaussianFilter()
    {
      auto gauss = std::make_shared<GaussianBlurClass>();
      gauss->value = 30;
      return gauss;
    }

    std::shared_ptr<ImageRegions> getRegions()
    {
      auto regions = std::make_shared<ImageRegions>();
      regions->value = 300;
      return regions;
    }

    std::shared_ptr<appContext> getContext()
    {
      return std::make_unique<appContext>(std::make_shared<MonoappB>());
    }
};

/* Concrete Builder for biespectral app */
class StereoBuilder_A : public appBuilder
{
  public:
    std::shared_ptr<IIRFilter>  getIIRFilter()
    {
      auto iir = std::make_shared<IIRFilter>();
      iir->value = 2;
      return iir;
    }

    std::shared_ptr<GaussianBlurClass> getGaussianFilter()
    {
      auto gauss = std::make_shared<GaussianBlurClass>();
      gauss->value = 20;
      return gauss;
    }

    std::shared_ptr<ImageRegions> getRegions()
    {
      auto regions = std::make_shared<ImageRegions>();
      regions->value = 200;
      return regions;
    }

    std::shared_ptr<appContext> getContext()
    {
      return std::make_unique<appContext>(std::make_shared<StereoappA>());
    }
};

/* Concrete Builder for biespectral app */
class StereoBuilder_B : public appBuilder
{
  public:
    std::shared_ptr<IIRFilter>  getIIRFilter()
    {
      auto iir = std::make_shared<IIRFilter>();
      iir->value = 4;
      return iir;
    }

    std::shared_ptr<GaussianBlurClass> getGaussianFilter()
    {
      auto gauss = std::make_shared<GaussianBlurClass>();
      gauss->value = 40;
      return gauss;
    }

    std::shared_ptr<ImageRegions> getRegions()
    {
      auto regions = std::make_shared<ImageRegions>();
      regions->value = 400;
      return regions;
    }

    std::shared_ptr<appContext> getContext()
    {
      return std::make_unique<appContext>(std::make_shared<StereoappB>());
    }
};


class appStrategy
{
public:
    virtual ~appStrategy() 
    {
      std::cout << "~appStrategy\n";
    }

    virtual std::string DoAlgorithm(const std::vector<std::string> &data) const = 0;
};

/**
 * The Context defines the interface of interest to clients.
 */

class appContext
{
    /**
     * @var Strategy The Context maintains a reference to one of the Strategy
     * objects. The Context does not know the concrete class of a strategy. It
     * should work with all strategies via the Strategy interface.
     */
private:
    std::shared_ptr<appStrategy> strategy_;
    /**
     * Usually, the Context accepts a strategy through the constructor, but also
     * provides a setter to change it at runtime.
     */
public:
    appContext(std::shared_ptr<appStrategy> strategy = nullptr) : strategy_(strategy)
    {
    }

    ~appContext()
    {
      std::cout << "~appContext\n";
    }
    
    /**
     * Usually, the Context allows replacing a Strategy object at runtime.
     */
    void set_strategy(std::shared_ptr<appStrategy> strategy)
    {
        this->strategy_ = strategy;
    }
    /**
     * The Context delegates some work to the Strategy object instead of
     * implementing +multiple versions of the algorithm on its own.
     */
    void DoSomeBusinessLogic() const
    {
        // ...
        std::cout << "Context: Sorting data using the strategy (not sure how it'll do it)\n";
        std::string result = this->strategy_->DoAlgorithm(std::vector<std::string>{"a", "e", "c", "b", "d"});
        std::cout << result << "\n";
        // ...
    }
};

/**
 * Concrete Strategies implement the algorithm while following the base Strategy
 * interface. The interface makes them interchangeable in the Context.
 */
class MonoappA : public appStrategy
{
public:
    std::string DoAlgorithm(const std::vector<std::string> &data) const override
    {
        std::string result;
        std::for_each(std::begin(data), std::end(data), [&result](const std::string &letter) {
            result += letter;
        });
        std::sort(std::begin(result), std::end(result));

        return result;
    }
};

/**
 * Concrete Strategies implement the algorithm while following the base Strategy
 * interface. The interface makes them interchangeable in the Context.
 */
class MonoappB : public appStrategy
{
    std::string DoAlgorithm(const std::vector<std::string> &data) const override
    {
        std::string result;
        std::for_each(std::begin(data), std::end(data), [&result](const std::string &letter) {
            result += letter;
        });
        std::sort(std::begin(result), std::end(result));
        for (int i = 0; i < result.size() / 2; i++)
        {
            std::swap(result[i], result[result.size() - i - 1]);
        }

        return result;
    }
};

/**
 * Concrete Strategies implement the algorithm while following the base Strategy
 * interface. The interface makes them interchangeable in the Context.
 */
class StereoappA : public appStrategy
{
public:
    std::string DoAlgorithm(const std::vector<std::string> &data) const override
    {
        std::string result;
        std::for_each(std::begin(data), std::end(data), [&result](const std::string &letter) {
            result += letter;
        });
        std::sort(std::begin(result), std::end(result));

        return result;
    }
};

/**
 * Concrete Strategies implement the algorithm while following the base Strategy
 * interface. The interface makes them interchangeable in the Context.
 */
class StereoappB : public appStrategy
{
    std::string DoAlgorithm(const std::vector<std::string> &data) const override
    {
        std::string result;
        std::for_each(std::begin(data), std::end(data), [&result](const std::string &letter) {
            result += letter;
        });
        std::sort(std::begin(result), std::end(result));
        for (int i = 0; i < result.size() / 2; i++)
        {
            std::swap(result[i], result[result.size() - i - 1]);
        }

        return result;
    }
};

/**
 * The client code picks a concrete strategy and passes it to the context. The
 * client should be aware of the differences between strategies in order to make
 * the right choice.
 */

void ClientCode()
{
  std::unique_ptr<app> app; // Final product

  /* A director who controls the process */
  Director director;

  /* Concrete builders */
  auto monoBuilder_A = std::make_shared<MonoBuilder_A>();
  auto monoBuilder_B = std::make_shared<MonoBuilder_B>();
  auto stereoBuilder_A = std::make_shared<StereoBuilder_A>();
  auto stereoBuilder_B = std::make_shared<StereoBuilder_B>();

  /* Build a monoespectral app A*/
  std::cout << "Monoespectral app A" << std::endl;
  director.setBuilder(monoBuilder_A); // using MonoBuilder_A instance
  app = director.getapp();
  std::cout << app << std::endl;
  std::cout << "Client: appStrategy is set to normal sorting.\n";
  app->context->DoSomeBusinessLogic();

  /* Build a monoespectral app B*/
  std::cout << "Monoespectral app B" << std::endl;
  director.setBuilder(monoBuilder_B); // using MonoBuilder_B instance
  app = director.getapp();
  std::cout << app << std::endl;
  std::cout << "Client: appStrategy is set to reverse sorting.\n";
  app->context->DoSomeBusinessLogic();

  /* Build a biespectral app A*/
  std::cout << "Biespectral app A" << std::endl;
  director.setBuilder(stereoBuilder_A); // using StereoBuilder_A instance
  app = director.getapp();
  std::cout << app << std::endl;
  std::cout << "Client: appStrategy is set to normal sorting.\n";
  app->context->DoSomeBusinessLogic();

  /* Build a biespectral app B*/
  std::cout << "Biespectral app B" << std::endl;
  director.setBuilder(stereoBuilder_B); // using StereoBuilder_B instance
  app = director.getapp();
  std::cout << app << std::endl;
  std::cout << "Client: appStrategy is set to reverse sorting.\n";
  app->context->DoSomeBusinessLogic();

}

int main()
{
  ClientCode();
  return 0;
}
